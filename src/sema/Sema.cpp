//
// Created by Michael Grossniklaus on 12/17/23.
//

#include "Sema.h"

#include <memory>
#include <set>
#include <string>

using std::make_unique;
using std::unique_ptr;
using std::set;
using std::string;

Sema::Sema(CompilerConfig &config, ASTContext *context, OberonSystem *system) :
        config_(config), context_(context), system_(system), logger_(config_.logger()), forwards_(), module_(), procs_(),
        symbols_(system_->getSymbolTable()), importer_(config_, context), exporter_(config_, context) {
    tBoolean_ = symbols_->getBasicType(to_string(TypeKind::BOOLEAN));
    tByte_ = symbols_->getBasicType(to_string(TypeKind::BYTE));
    tChar_ = symbols_->getBasicType(to_string(TypeKind::CHAR));
    tInteger_ = symbols_->getBasicType(to_string(TypeKind::INTEGER));
    tLongInt_ = symbols_->getBasicType(to_string(TypeKind::LONGINT));
    tReal_ = symbols_->getBasicType(to_string(TypeKind::REAL));
    tLongReal_ = symbols_->getBasicType(to_string(TypeKind::LONGREAL));
    tString_ = symbols_->getBasicType(to_string(TypeKind::STRING));
}

void
Sema::onTranslationUnitStart(const string &name) {
    symbols_->createNamespace(name, true);
}

void
Sema::onTranslationUnitEnd(const string &name) {
    if (logger_.getErrorCount() == 0) {
        exporter_.write(name, symbols_);
    }
}

void Sema::onBlockStart() {
    forwards_.clear();
    symbols_->openScope();
}

void Sema::onBlockEnd() {
    symbols_->closeScope();
    if (!forwards_.empty()) {
        for (const auto& pair: forwards_) {
            auto base = pair.second->getBase();
            logger_.error(base->pos(), "undefined forward reference: " + format(base) + ".");
        }
    }
}

ModuleNode *
Sema::onModuleStart(const FilePos &start, unique_ptr<Ident> ident, vector<unique_ptr<ImportNode>> imports) {
    module_ = make_unique<ModuleNode>(start, std::move(ident), std::move(imports));
    assertUnique(module_->getIdentifier(), module_.get());
    module_->setLevel(symbols_->getLevel());
    onBlockStart();
    return module_.get();
}

unique_ptr<ModuleNode> Sema::onModuleEnd([[maybe_unused]] const FilePos &end, unique_ptr<Ident> ident) {
    onBlockEnd();
    if (*module_->getIdentifier() != *ident.get()) {
        logger_.error(ident->start(), "module name mismatch: expected " + to_string(*module_->getIdentifier()) +
                                       ", found " + to_string(*ident) + ".");
    }
    return std::move(module_);
}


unique_ptr<ImportNode>
Sema::onImport(const FilePos &start, [[maybe_unused]] const FilePos &end,
               unique_ptr<Ident> alias, unique_ptr<Ident> ident) {
    auto node = make_unique<ImportNode>(start, std::move(alias), std::move(ident));
    // TODO check duplicate imports
    std::unique_ptr<ModuleNode> module;
    if (node->getAlias()) {
        module = importer_.read(node->getAlias()->name(), node->getModule()->name(), symbols_);
    } else {
        module = importer_.read(node->getModule()->name(), symbols_);
    }
    if (module) {
        context_->addExternalModule(std::move(module));
    } else {
        logger_.error(node->pos(), "module " + node->getModule()->name() + " could not be imported.");
    }
    return node;
}

unique_ptr<ConstantDeclarationNode>
Sema::onConstant(const FilePos &start, [[maybe_unused]] const FilePos &end,
                 unique_ptr<IdentDef> ident, unique_ptr<ExpressionNode> expr) {
    if (expr) {
        if (!expr->isConstant()) {
            logger_.error(expr->pos(), "value must be constant.");
        }
        if (!expr->isLiteral()) {
            logger_.error(expr->pos(), "undefined constant.");
        }
    } else {
        logger_.error(start, "undefined constant.");
    }
    auto node = make_unique<ConstantDeclarationNode>(start, std::move(ident), std::move(expr));
    assertUnique(node->getIdentifier(), node.get());
    node->setLevel(symbols_->getLevel());
    node->setModule(module_.get());
    checkExport(node.get());
    return node;
}

unique_ptr<TypeDeclarationNode>
Sema::onType(const FilePos &start, [[maybe_unused]] const FilePos &end,
             unique_ptr<IdentDef> ident, TypeNode *type) {
    auto node = make_unique<TypeDeclarationNode>(start, std::move(ident), type);
    assertUnique(node->getIdentifier(), node.get());
    node->setLevel(symbols_->getLevel());
    node->setModule(module_.get());
    checkExport(node.get());
    if (type) {
        auto it = forwards_.find(node->getIdentifier()->name());
        if (it != forwards_.end()) {
            auto pointer_t = it->second;
            logger_.debug("Resolving forward reference: " + to_string(*node->getIdentifier()));
            pointer_t->setBase(type);
            forwards_.erase(it);
        }
    }
    return node;
}

ArrayTypeNode *
Sema::onArrayType(const FilePos &start, [[maybe_unused]] const FilePos &end,
                  Ident *ident, unique_ptr<ExpressionNode> expr, TypeNode *type) {
    unsigned dim = 0;
    if (expr) {
        if (!expr->isConstant()) {
            logger_.error(expr->pos(), "constant expression expected.");
        }
        if (expr->getType()) {
            if (expr->isLiteral()) {
                if (expr->getType()->isInteger()) {
                    auto literal = dynamic_cast<const IntegerLiteralNode *>(expr.get());
                    if (literal->value() <= 0) {
                        logger_.error(expr->pos(), "array dimension must be a positive value.");
                    }
                    dim = (unsigned) literal->value();
                } else {
                    logger_.error(expr->pos(), "integer expression expected.");
                }
            } else {
                logger_.error(expr->pos(), "constant integer expression expected.");
            }
        } else {
            logger_.error(expr->pos(), "undefined array dimension type.");
        }
    } else {
        logger_.error(start, "undefined array dimension.");
    }
    if (!type) {
        logger_.error(start, "undefined member type.");
    }
    auto res = context_->getOrInsertArrayType(ident, dim, type);
    if (type->getSize() > 0 && dim > 0) {
        res->setSize(dim * type->getSize());
    }
    return res;
}

PointerTypeNode *
Sema::onPointerType(const FilePos &start, const FilePos &end, Ident *ident, unique_ptr<QualIdent> reference) {
    auto sym = symbols_->lookup(reference.get());
    if (!sym) {
        auto node = context_->getOrInsertPointerType(ident, nullptr);
        forwards_[reference->name()] = node;
        logger_.debug("Found possible forward type reference: " + to_string(*reference));
        return node;
    } else {
        auto type = onTypeReference(start, end, std::move(reference));
        return onPointerType(start, end, ident, type);
    }
}

PointerTypeNode *
Sema::onPointerType([[maybe_unused]] const FilePos &start, [[maybe_unused]] const FilePos &end,
                                     Ident *ident, TypeNode *base) {
    return context_->getOrInsertPointerType(ident, base);
}

ProcedureTypeNode *
Sema::onProcedureType([[maybe_unused]] const FilePos &start, [[maybe_unused]] const FilePos &end,
                      Ident *ident, vector<unique_ptr<ParameterNode>> params, bool varargs, TypeNode *ret) {
    return context_->getOrInsertProcedureType(ident, std::move(params), varargs, ret);
}

unique_ptr<ParameterNode>
Sema::onParameter(const FilePos &start, [[maybe_unused]] const FilePos &end,
                  unique_ptr<Ident> ident, TypeNode *type, bool is_var, unsigned index) {
    if (!type) {
        logger_.error(start, "undefined parameter type.");
    }
    auto node = make_unique<ParameterNode>(start, std::move(ident), type, is_var, index);
    assertUnique(node->getIdentifier(), node.get());
    node->setLevel(symbols_->getLevel());
    return node;
}

RecordTypeNode *
Sema::onRecordType([[maybe_unused]] const FilePos &start, [[maybe_unused]] const FilePos &end,
                  Ident *ident, vector<unique_ptr<FieldNode>> fields) {
    if (fields.empty()) {
        logger_.error(start, "records needs at least one field.");
    }
    auto node = context_->getOrInsertRecordType(ident, std::move(fields));
    set<string> names;
    for (size_t i = 0; i < node->getFieldCount(); i++) {
        auto field = node->getField(i);
        if (field->getIdentifier()->isExported()) {
            if (symbols_->getLevel() != SymbolTable::MODULE_LEVEL) {
                logger_.error(field->pos(), "only top-level declarations can be exported.");
            } else if (!node->getIdentifier()->isExported()) {
                logger_.error(field->pos(), "cannot export fields of non-exported record type.");
            }
        }
        if (names.count(field->getIdentifier()->name())) {
            logger_.error(field->pos(), "duplicate record field: " + to_string(*field->getIdentifier()) + ".");
        } else {
            names.insert(field->getIdentifier()->name());
        }
    }
    return node;
}

unique_ptr<FieldNode>
Sema::onField(const FilePos &start, [[maybe_unused]] const FilePos &end,
              unique_ptr<IdentDef> ident, TypeNode *type, unsigned index) {
    if (!type) {
        logger_.error(start, "undefined record field type.");
    }
    return make_unique<FieldNode>(start, std::move(ident), type, index);
}

TypeNode *
Sema::onTypeReference(const FilePos &start, [[maybe_unused]] const FilePos &end, unique_ptr<QualIdent> ident) {
    auto sym = symbols_->lookup(ident.get());
    if (sym) {
        if (sym->getNodeType() == NodeType::array_type ||
            sym->getNodeType() == NodeType::basic_type ||
            sym->getNodeType() == NodeType::record_type ||
            sym->getNodeType() == NodeType::pointer_type) {
            return dynamic_cast<TypeNode *>(sym);
        }
        if (sym->getNodeType() == NodeType::type) {
            auto type = dynamic_cast<TypeDeclarationNode *>(sym);
            return type->getType();
        }
        logger_.error(start, to_string(*ident) + " is not a type.");
    } else {
        logger_.error(start, "undefined type: " + to_string(*ident) + ".");
    }
    return nullptr;
}

unique_ptr<VariableDeclarationNode>
Sema::onVariable(const FilePos &start, [[maybe_unused]] const FilePos &end,
                 unique_ptr<IdentDef> ident, TypeNode *type, int index) {
    if (!type) {
        logger_.error(start, "undefined variable type.");
    }
    auto node = make_unique<VariableDeclarationNode>(start, std::move(ident), type, index);
    assertUnique(node->getIdentifier(), node.get());
    node->setLevel(symbols_->getLevel());
    node->setModule(module_.get());
    checkExport(node.get());
    return node;
}

ProcedureNode *
Sema::onProcedureStart(const FilePos &start, unique_ptr<IdentDef> ident) {
    procs_.push(make_unique<ProcedureNode>(start, std::move(ident)));
    auto proc = procs_.top().get();
    assertUnique(proc->getIdentifier(), proc);
    proc->setLevel(symbols_->getLevel());
    proc->setModule(module_.get());
    checkExport(proc);
    onBlockStart();
    return proc;
}

unique_ptr<ProcedureNode>
Sema::onProcedureEnd([[maybe_unused]] const FilePos &end, unique_ptr<Ident> ident) {
    onBlockEnd();
    auto proc = std::move(procs_.top());
    procs_.pop();
    if (proc->isExtern()) {
        if (proc->getLevel() != SymbolTable::MODULE_LEVEL) {
            logger_.error(proc->pos(), "only top-level procedures can be external.");
        }
    } else {
        if (*proc->getIdentifier() != *ident) {
            logger_.error(ident->start(), "procedure name mismatch: expected " + to_string(*proc->getIdentifier()) +
                                           ", found " + to_string(*ident) + ".");
        }
    }
    return proc;
}

unique_ptr<AssignmentNode>
Sema::onAssignment(const FilePos &start, [[maybe_unused]] const FilePos &end,
                   unique_ptr<QualifiedExpression> lvalue, unique_ptr<ExpressionNode> rvalue) {
    if (!lvalue) {
        logger_.error(start, "undefined left-hand side in assignment.");
    }
    auto decl = lvalue->dereference();
    if (decl) {
        if (decl->getNodeType() == NodeType::constant) {
            logger_.error(lvalue->pos(), "cannot assign constant.");
        }
    } else {
        logger_.error(lvalue->pos(), "undefined left-hand side in assignment.");
    }
    if (!rvalue) {
        logger_.error(start, "undefined right-hand side in assignment.");
    }
    if (lvalue && rvalue) {
        // Type inference: check that types are compatible
        if (assertCompatible(lvalue->pos(), lvalue->getType(), rvalue->getType())) {
            cast(rvalue.get(), lvalue->getType());
        }
    }
    return make_unique<AssignmentNode>(start, std::move(lvalue), std::move(rvalue));
}

unique_ptr<IfThenElseNode>
Sema::onIfStatement(const FilePos &start, [[maybe_unused]] const FilePos &end,
                    unique_ptr<ExpressionNode> condition,
                    unique_ptr<StatementSequenceNode> thenStmts,
                    vector<unique_ptr<ElseIfNode>> elseIfs,
                    unique_ptr<StatementSequenceNode> elseStmts) {
    if (!condition) {
        logger_.error(start, "undefined condition in if-statement.");
    }
    auto type = condition->getType();
    if (type && type->kind() != TypeKind::BOOLEAN) {
        logger_.error(condition->pos(), "Boolean expression expected.");
    }
    return make_unique<IfThenElseNode>(start, std::move(condition), std::move(thenStmts),
                                       std::move(elseIfs), std::move(elseStmts));
}

unique_ptr<ElseIfNode>
Sema::onElseIf(const FilePos &start, [[maybe_unused]] const FilePos &end,
               unique_ptr<ExpressionNode> condition,
               unique_ptr<StatementSequenceNode> stmts) {
    if (!condition) {
        logger_.error(start, "undefined condition in elsif-statement.");
    }
    auto type = condition->getType();
    if (type && type->kind() != TypeKind::BOOLEAN) {
        logger_.error(condition->pos(), "Boolean expression expected.");
    }
    return make_unique<ElseIfNode>(start, std::move(condition), std::move(stmts));
}

unique_ptr<LoopNode>
Sema::onLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
             unique_ptr<StatementSequenceNode> stmts) {
    return make_unique<LoopNode>(start, std::move(stmts));
}

unique_ptr<RepeatLoopNode>
Sema::onRepeatLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
                   unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts) {
    if (!condition) {
        logger_.error(start, "undefined condition in repeat-loop.");
    }
    auto type = condition->getType();
    if (type && type->kind() != TypeKind::BOOLEAN) {
        logger_.error(condition->pos(), "Boolean expression expected.");
    }
    return make_unique<RepeatLoopNode>(start, std::move(condition), std::move(stmts));
}

unique_ptr<WhileLoopNode>
Sema::onWhileLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
                  unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts) {
    if (!condition) {
        logger_.error(start, "undefined condition in while-loop.");
    }
    auto type = condition->getType();
    if (type && type->kind() != TypeKind::BOOLEAN) {
        logger_.error(condition->pos(), "Boolean expression expected.");
    }
    return make_unique<WhileLoopNode>(start, std::move(condition), std::move(stmts));
}

unique_ptr<ForLoopNode>
Sema::onForLoop(const FilePos &start, [[maybe_unused]] const FilePos &end,
                unique_ptr<QualIdent> var,
                unique_ptr<ExpressionNode> low, unique_ptr<ExpressionNode> high, unique_ptr<ExpressionNode> step,
                unique_ptr<StatementSequenceNode> stmts) {
    // auto ident = to_string(*var);
    vector<unique_ptr<Selector>> selectors;
    auto counter = onQualifiedExpression(var->start(), var->end(), std::move(var), std::move(selectors));
    if (!counter) {
        logger_.error(start, "undefined counter variable in for-loop.");
    }
    auto ident = counter->ident();
    if (ident->isQualified()) {
        logger_.error(ident->start(), to_string(*ident) + " cannot be used as a loop counter.");
    }
    if (counter->getNodeType() == NodeType::qualified_expression) {
        auto decl = dynamic_cast<QualifiedExpression *>(counter.get())->dereference();
        if (decl->getNodeType() != NodeType::variable) {
            logger_.error(ident->start(), "variable expected.");
        }
        auto type = decl->getType();
        if (type && type->kind() != TypeKind::INTEGER) {
            logger_.error(ident->start(), "integer variable expected.");
        }
    } else {
        logger_.error(ident->start(), to_string(*ident) + " cannot be used as a loop counter.");
    }
    if (!low) {
        logger_.error(start, "undefined low value in for-loop.");
    }
    auto type = low->getType();
    if (type && type->kind() != TypeKind::INTEGER) {
        logger_.error(low->pos(), "integer expression expected.");
    }
    if (!high) {
        logger_.error(start, "undefined high value in for-loop.");
    }
    type = high->getType();
    if (type && type->kind() != TypeKind::INTEGER) {
        logger_.error(high->pos(), "integer expression expected.");
    }
    if (step && step->isLiteral()) {
        type = step->getType();
        if (type && type->kind() == TypeKind::INTEGER && step->isConstant()) {
            auto val = dynamic_cast<IntegerLiteralNode *>(step.get())->value();
            if (val == 0) {
                logger_.error(step->pos(), "step value cannot be zero.");
            }
        } else {
            logger_.error(step->pos(), "constant integer expression expected.");
        }
    }
    return make_unique<ForLoopNode>(start, std::move(counter), std::move(low), std::move(high), std::move(step),
                                    std::move(stmts));
}

unique_ptr<ReturnNode>
Sema::onReturn(const FilePos &start, [[maybe_unused]] const FilePos &end, unique_ptr<ExpressionNode> expr) {
    if (procs_.empty()) {
        if (expr) {
            logger_.error(expr->pos(), "module cannot return a value.");
        }
    } else {
        auto proc = procs_.top().get();
        if (expr) {
            if (!proc->getType()->getReturnType()) {
                logger_.error(expr->pos(), "procedure cannot return a value.");
            }
            if (assertCompatible(expr->pos(), proc->getType()->getReturnType(), expr->getType())) {
                cast(expr.get(), proc->getType()->getReturnType());
            }
        } else {
            if (proc->getType()->getReturnType()) {
                logger_.error(proc->pos(), "function must return value.");
            }
        }
    }
    return make_unique<ReturnNode>(start, std::move(expr));
}

unique_ptr<StatementNode>
Sema::onQualifiedStatement(const FilePos &start, [[maybe_unused]] const FilePos &end,
                           unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors) {
    DeclarationNode* sym = symbols_->lookup(ident.get());
    if (!sym) {
        logger_.error(ident->start(), "undefined identifier: " + to_string(*ident) + ".");
        return nullptr;
    }
    // check procedure reference
    if (sym->getNodeType() == NodeType::procedure) {
        auto proc = dynamic_cast<ProcedureNode *>(sym);
        if (ident->isQualified() && proc->isExtern()) {
            // a fully-qualified external reference needs to be added to module for code generation
            context_->addExternalProcedure(proc);
        }
        auto type = onSelectors(sym->getType(), selectors);
        if (type) {
            logger_.warning(ident->start(), "discarded expression value.");
        }
        return make_unique<QualifiedStatement>(start, std::move(ident), std::move(selectors), sym);
    }
    logger_.error(ident->start(), "procedure call expected.");
    return nullptr;
}

unique_ptr<QualifiedExpression>
Sema::onQualifiedExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                            unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors) {
    DeclarationNode* sym = symbols_->lookup(ident.get());
    if (!sym) {
        logger_.error(ident->start(), "undefined identifier: " + to_string(*ident) + ".");
        return nullptr;
    }
    // check variable or parameter reference
    if (sym->getNodeType() == NodeType::variable || sym->getNodeType() == NodeType::parameter) {
        auto type = onSelectors(sym->getType(), selectors);
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, type);
    }
    // check procedure reference
    if (sym->getNodeType() == NodeType::procedure) {
        auto proc = dynamic_cast<ProcedureNode *>(sym);
        if (ident->isQualified() && proc->isExtern()) {
            // a fully-qualified external reference needs to be added to module for code generation
            context_->addExternalProcedure(proc);
        }
        auto type = onSelectors(sym->getType(), selectors);
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, type);
    }
    logger_.error(ident->start(), "variable, parameter, or function call expected.");
    return nullptr;
}

unique_ptr<LiteralNode>
Sema::onQualifiedConstant(const FilePos &start, const FilePos &end,
                          unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors) {
    DeclarationNode* sym = symbols_->lookup(ident.get());
    if (!sym) {
        logger_.error(ident->start(), "undefined identifier: " + to_string(*ident) + ".");
        return nullptr;
    }
    // check constant reference
    if (sym->getNodeType() == NodeType::constant) {
        if (!selectors.empty()) {
            auto sel = selectors[0].get();
            logger_.warning(sel->pos(), "ignoring unexpected selector(s).");
        }
        auto decl = dynamic_cast<ConstantDeclarationNode *>(sym);
        return fold(start, end, decl->getValue());
    }
    logger_.error(ident->start(), "constant expected.");
    return nullptr;
}

TypeNode *Sema::onSelectors(TypeNode *base, vector<unique_ptr<Selector>> &selectors) {
    auto it = selectors.begin();
    it = handleMissingParameters(base, selectors, it);
    while (it != selectors.end()) {
        auto sel = (*it).get();
        if (!base) {
            logger_.error(sel->pos(), "unexpected selector.");
            return nullptr;
        }
        // check for implicit pointer de-referencing
        if (base->isPointer() && (sel->getNodeType() == NodeType::array_type ||
                                  sel->getNodeType() == NodeType::record_type)) {
            auto caret = std::make_unique<Dereference>(sel->pos());
            // place caret before the current element
            it = selectors.insert(it, std::move(caret));
            sel = (*it).get();
        }
        switch (sel->getNodeType()) {
            case NodeType::parameter:
                base = onActualParameters(base, dynamic_cast<ActualParameters *>(sel));
                break;
            case NodeType::array_type:
                base = onArrayIndex(base, dynamic_cast<ArrayIndex *>(sel));
                break;
            case NodeType::pointer_type:
                base = onDereference(base, dynamic_cast<Dereference *>(sel));
                break;
            case NodeType::record_type:
                base = onRecordField(base, dynamic_cast<RecordField *>(sel));
                break;
            case NodeType::type:
                base = onTypeguard(base, dynamic_cast<Typeguard *>(sel));
                break;
            default:
                logger_.error(sel->pos(), "unexpected selector.");
                return nullptr;
        }
        it = handleMissingParameters(base, selectors, it);
        ++it;
    }
    return base;
}

Sema::SelectorIterator &
Sema::handleMissingParameters(TypeNode *base, Sema::Selectors &selectors, Sema::SelectorIterator &it) {
    if (base && base->isProcedure()) {
        if (selectors.empty()) {
            it = selectors.insert(selectors.begin(), make_unique<ActualParameters>());
        } else if (it + 1 != selectors.end() && (*(it +1))->getNodeType() != NodeType::parameter) {
            it = selectors.insert(it, make_unique<ActualParameters>());
        }
    }
    return it;
}

TypeNode *Sema::onActualParameters(TypeNode *base, ActualParameters *sel) {
    if (!base->isProcedure()) {
        logger_.error(sel->pos(), "type " + to_string(base) + " is not a procedure type.");
        return nullptr;
    }
    auto proc = dynamic_cast<ProcedureTypeNode *>(base);
    if (sel->parameters().size() < proc->parameters().size()) {
        logger_.error(sel->pos(), "fewer actual than formal parameters.");
    }
    for (size_t cnt = 0; cnt < sel->parameters().size(); cnt++) {
        auto expr = sel->parameters()[cnt].get();
        if (cnt < proc->parameters().size()) {
            auto param = proc->parameters()[cnt].get();
            if (assertCompatible(expr->pos(), param->getType(), expr->getType(), param->isVar())) {
                if (param->isVar()) {
                    if (expr->isLiteral()) {
                        logger_.error(expr->pos(), "illegal actual parameter: cannot pass constant by reference.");
                    } else if (expr->getNodeType() == NodeType::qualified_expression) {
                        continue;
                    } else {
                        logger_.error(expr->pos(), "illegal actual parameter: cannot pass expression by reference.");
                    }
                } else {
                    cast(expr, param->getType());
                }
            }
        } else if (!proc->hasVarArgs()) {
            logger_.error(expr->pos(), "more actual than formal parameters.");
            break;
        }
    }
    return proc->getReturnType();
}

TypeNode *Sema::onArrayIndex(TypeNode *base, ArrayIndex *sel) {
    if (!base->isArray()) {
        logger_.error(sel->pos(), "type " + to_string(base) + " is not an array type.");
        return nullptr;
    }
    auto type = dynamic_cast<ArrayTypeNode *>(base);
    if (sel->indices().size() > 1) {
        logger_.error(sel->pos(), "multi-dimensional arrays are not yet supported.");
    }
    for (auto& index : sel->indices()) {
        auto sel_type = index->getType();
        if (sel_type && sel_type->kind() != TypeKind::INTEGER) {
            logger_.error(sel->pos(), "integer expression expected.");
        }
    }
    return type->getMemberType();
}

TypeNode *Sema::onDereference(TypeNode *base, Dereference *sel) {
    if (!base->isPointer()) {
        logger_.error(sel->pos(), "type " + to_string(base) + " is not a pointer type.");
        return nullptr;
    }
    auto type = dynamic_cast<PointerTypeNode *>(base);
    return type->getBase();
}

TypeNode *Sema::onRecordField(TypeNode *base, RecordField *sel) {
    if (!base->isRecord()) {
        logger_.error(sel->pos(), "type " + to_string(base) + " is not a record type.");
        return nullptr;
    }
    auto type = dynamic_cast<RecordTypeNode *>(base);
    auto ref = dynamic_cast<RecordField *>(sel);
    auto field = type->getField(ref->ident()->name());
    if (!field) {
        logger_.error(ref->pos(), "undefined record field: " + to_string(*ref->ident()) + ".");
        return nullptr;
    } else {
        ref->setField(field);
        return field->getType();
    }
}

TypeNode *Sema::onTypeguard([[maybe_unused]] TypeNode *base, Typeguard *sel) {
    auto sym = symbols_->lookup(sel->ident());
    if (sym) {
        if (sym->getNodeType() == NodeType::type) {
            // TODO check if type-guard is compatible with base type.
            return dynamic_cast<TypeDeclarationNode *>(sym)->getType();
        } else {
            logger_.error(sel->pos(), "unexpected selector.");
        }
    } else {
        logger_.error(sel->pos(), "undefined identifier: " + to_string(*sel->ident()) + ".");
    }
    return nullptr;
}

unique_ptr<ExpressionNode>
Sema::onUnaryExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                        OperatorType op, unique_ptr<ExpressionNode> expr) {
    if (!expr) {
        logger_.error(start, "undefined expression in unary expression.");
        return nullptr;
    }
    auto type = expr->getType();
    if (!type) {
        logger_.error(start, "undefined type in unary expression.");
        return nullptr;
    }
    if (expr->isConstant()) {
        return fold(start, end, op, expr.get());
    }
    return make_unique<UnaryExpressionNode>(start, op, std::move(expr), type);
}

unique_ptr<ExpressionNode>
Sema::onBinaryExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                         OperatorType op, unique_ptr<ExpressionNode> lhs, unique_ptr<ExpressionNode> rhs) {
    if (!lhs) {
        logger_.error(start, "undefined left-hand side in binary expression.");
        return nullptr;
    }
    if (!rhs) {
        logger_.error(start, "undefined right-hand side in binary expression.");
        return nullptr;
    }
    auto lhsType = lhs->getType();
    if (!lhsType) {
        logger_.error(lhs->pos(), "undefined left-hand side type in binary expression.");
        return nullptr;
    }
    auto rhsType = rhs->getType();
    if (!rhsType) {
        logger_.error(lhs->pos(), "undefined right-hand side type in binary expression.");
        return nullptr;
    }
    // Type inference: find the common type of lhs and rhs, if one exists
    auto common = commonType(lhsType, rhsType);
    if (!common) {
        logger_.error(start, "incompatible types (" + lhsType->getIdentifier()->name() + ", " +
                             rhsType->getIdentifier()->name() + ")");
        return nullptr;
    }
    // Type inference: except for boolean comparisons and floating-point division,
    // the result type is the common type of lhs and rhs
    auto type = common;
    if (op == OperatorType::EQ
        || op == OperatorType::NEQ
        || op == OperatorType::LT
        || op == OperatorType::LEQ
        || op == OperatorType::GT
        || op == OperatorType::GEQ) {
        type = this->tBoolean_;
    } else if (op == OperatorType::DIV && (!lhsType->isInteger() || !rhsType->isInteger())) {
        logger_.error(start, "integer division needs integer arguments.");
        return nullptr;
    } else if (op == OperatorType::DIVIDE) {
        if (common->isInteger()) {
            if (common->kind() == TypeKind::LONGINT) {
                type = this->tLongReal_;
            } else {
                type = this->tReal_;
            }
        }
    }
    // Folding
    if (lhs->isConstant() && rhs->isConstant()) {
        return fold(start, end, op, lhs.get(), rhs.get(), type);
    }
    // Casting left-hand side to common type
    cast(lhs.get(), common);
    // Casting right-hand side to common type
    cast(rhs.get(), common);
    return make_unique<BinaryExpressionNode>(start, op, std::move(lhs), std::move(rhs), type);
}

unique_ptr<BooleanLiteralNode>
Sema::onBooleanLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, bool value) {
    return make_unique<BooleanLiteralNode>(start, value, this->tBoolean_);
}

unique_ptr<IntegerLiteralNode>
Sema::onIntegerLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, long value, bool ext) {
    TypeNode *type = ext ? this->tLongInt_ : this->tInteger_;
    return make_unique<IntegerLiteralNode>(start, value, type);
}

unique_ptr<RealLiteralNode>
Sema::onRealLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, double value, bool ext) {
    TypeNode *type = ext ? this->tLongReal_ : this->tReal_;
    return make_unique<RealLiteralNode>(start, value, type);
}

unique_ptr<StringLiteralNode>
Sema::onStringLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, const string &value) {
    return make_unique<StringLiteralNode>(start, value, this->tString_);
}

unique_ptr<NilLiteralNode>
Sema::onNilLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end) {
    return make_unique<NilLiteralNode>(start, symbols_->getNilType());
}

bool Sema::isDefined(Ident *ident) {
    return symbols_->lookup(ident) != nullptr;
}

bool Sema::isConstant(QualIdent *ident) {
    auto sym = symbols_->lookup(ident);
    return sym && sym->getNodeType() == NodeType::constant;
}

bool Sema::isType(QualIdent *ident) {
    auto sym = symbols_->lookup(ident);
    return sym && sym->getNodeType() == NodeType::type;
}

bool Sema::isVariable(QualIdent *ident) {
    auto sym = symbols_->lookup(ident);
    return sym && sym->getNodeType() == NodeType::variable;
}

bool Sema::isProcedure(QualIdent *ident) {
    auto sym = symbols_->lookup(ident);
    return sym && sym->getNodeType() == NodeType::procedure;
}

bool
Sema::foldBoolean(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    expr = resolveReference(expr);
    if (expr->getNodeType() == NodeType::boolean) {
        return dynamic_cast<const BooleanLiteralNode *>(expr)->value();
    }
    logger_.error(start, "expression is not a constant boolean value.");
    return false;
}

long
Sema::foldInteger(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    expr = resolveReference(expr);
    if (expr->getNodeType() == NodeType::integer) {
        return dynamic_cast<const IntegerLiteralNode *>(expr)->value();
    }
    logger_.error(start, "expression is not a constant integer value.");
    return 0;
}

double
Sema::foldReal(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    expr = resolveReference(expr);
    if (expr->getNodeType() == NodeType::real) {
        return dynamic_cast<const RealLiteralNode *>(expr)->value();
    } else if (expr->getNodeType() == NodeType::integer) {
        // promote integer to real
        return (double) foldInteger(start, end, expr);
    }
    logger_.error(start, "expression is not a constant real value.");
    return 0.0;
}

string
Sema::foldString(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    expr = resolveReference(expr);
    if (expr->getNodeType() == NodeType::string) {
        return dynamic_cast<const StringLiteralNode *>(expr)->value();
    }
    logger_.error(start, "expression is not a constant string value.");
    return "";
}

std::unique_ptr<LiteralNode>
Sema::fold(const FilePos &start, const FilePos &end, ExpressionNode *expr) {
    auto type = expr->getType();
    if (type) {
        auto cast = expr->getCast();
        if (type->kind() == TypeKind::INTEGER || type->kind() == TypeKind::LONGINT) {
            return std::make_unique<IntegerLiteralNode>(expr->pos(), foldInteger(start, end, expr), type, cast);
        } else if (type->kind() == TypeKind::REAL || type->kind() == TypeKind::LONGREAL) {
            return std::make_unique<RealLiteralNode>(expr->pos(), foldReal(start, end, expr), type, cast);
        } else if (type->kind() == TypeKind::BOOLEAN) {
            return std::make_unique<BooleanLiteralNode>(expr->pos(), foldBoolean(start, end, expr), type, cast);
        } else if (type->kind() == TypeKind::STRING) {
            return std::make_unique<StringLiteralNode>(expr->pos(), foldString(start, end, expr), type, cast);
        }
        logger_.error(start, "incompatible types.");
    } else {
        logger_.error(start, "undefined literal type.");
    }
    return nullptr;
}


unique_ptr<LiteralNode>
Sema::fold(const FilePos &start, [[maybe_unused]] const FilePos &end, OperatorType op, ExpressionNode *expr) {
    auto type = expr->getType();
    auto cast = expr->getCast();
    if (type->isBoolean()) {
        bool value = foldBoolean(start, end, expr);
        switch (op) {
            case OperatorType::NOT:
                return make_unique<BooleanLiteralNode>(start, !value, type, cast);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to boolean values.");
        }
    } else if (type->isInteger()) {
        long value = foldInteger(start, end, expr);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<IntegerLiteralNode>(start, value, type, cast);
            case OperatorType::NEG:
                return make_unique<IntegerLiteralNode>(start, -value, type, cast);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
        }
    } else if (type->isReal()) {
        double value = foldReal(start, end, expr);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<RealLiteralNode>(start, value, type, cast);
            case OperatorType::NEG:
                return make_unique<RealLiteralNode>(start, -value, type, cast);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to real values.");
        }
    }
    logger_.error(start, "invalid unary expression.");
    return nullptr;
}

unique_ptr<LiteralNode>
Sema::fold(const FilePos &start, [[maybe_unused]] const FilePos &end,
                           OperatorType op, ExpressionNode *lhs, ExpressionNode *rhs, TypeNode* common) {
    if (common->isBoolean()) {
        if (lhs->getType()->isBoolean() && rhs->getType()->isBoolean()) {
            bool lvalue = foldBoolean(start, end, lhs);
            bool rvalue = foldBoolean(start, end, rhs);
            switch (op) {
                case OperatorType::AND:
                    return make_unique<BooleanLiteralNode>(start, lvalue && rvalue, common);
                case OperatorType::OR:
                    return make_unique<BooleanLiteralNode>(start, lvalue || rvalue, common);
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to boolean values.");
            }
        } else if (lhs->getType()->isNumeric() && rhs->getType()->isNumeric()) {
            if (lhs->getType()->isInteger() && rhs->getType()->isInteger()) {
                long lvalue = foldInteger(start, end, lhs);
                long rvalue = foldInteger(start, end, rhs);
                switch (op) {
                    case OperatorType::EQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                    case OperatorType::NEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
                    case OperatorType::LT:
                        return make_unique<BooleanLiteralNode>(start, lvalue <= rvalue, common);
                    case OperatorType::LEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue < rvalue, common);
                    case OperatorType::GT:
                        return make_unique<BooleanLiteralNode>(start, lvalue > rvalue, common);
                    case OperatorType::GEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue >= rvalue, common);
                    default:
                        logger_.error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
                }
            } else {
                double lvalue = foldReal(start, end, lhs);
                double rvalue = foldReal(start, end, rhs);
                switch (op) {
                    case OperatorType::EQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                    case OperatorType::NEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
                    case OperatorType::LT:
                        return make_unique<BooleanLiteralNode>(start, lvalue <= rvalue, common);
                    case OperatorType::LEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue < rvalue, common);
                    case OperatorType::GT:
                        return make_unique<BooleanLiteralNode>(start, lvalue > rvalue, common);
                    case OperatorType::GEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue >= rvalue, common);
                    default:
                        logger_.error(start, "operator " + to_string(op) + " cannot be applied to real values.");
                }
            }
        } else if (lhs->getType()->isString() && rhs->getType()->isString()) {
            string lvalue = foldString(start, end, lhs);
            string rvalue = foldString(start, end, rhs);
            switch (op) {
                case OperatorType::EQ:
                    return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                case OperatorType::NEQ:
                    return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to string values.");
            }
        } else if (lhs->getType()->kind() == TypeKind::NILTYPE && rhs->getType()->kind() == TypeKind::NILTYPE) {
            switch (op) {
                case OperatorType::EQ:
                    return make_unique<BooleanLiteralNode>(start, true, common);
                case OperatorType::NEQ:
                    return make_unique<BooleanLiteralNode>(start, false, common);
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to NIL values.");
            }
        }
    } else if (common->isInteger()) {
        long lvalue = foldInteger(start, end, lhs);
        long rvalue = foldInteger(start, end, rhs);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<IntegerLiteralNode>(start, lvalue + rvalue, common);
            case OperatorType::MINUS:
                return make_unique<IntegerLiteralNode>(start, lvalue - rvalue, common);
            case OperatorType::TIMES:
                return make_unique<IntegerLiteralNode>(start, lvalue * rvalue, common);
            case OperatorType::DIV:
                return make_unique<IntegerLiteralNode>(start, lvalue / rvalue, common);
            case OperatorType::MOD:
                return make_unique<IntegerLiteralNode>(start, lvalue % rvalue, common);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
        }
    } else if (common->isReal()) {
        double lvalue = foldReal(start, end, lhs);
        double rvalue = foldReal(start, end, rhs);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<RealLiteralNode>(start, lvalue + rvalue, common);
            case OperatorType::MINUS:
                return make_unique<RealLiteralNode>(start, lvalue - rvalue, common);
            case OperatorType::TIMES:
                return make_unique<RealLiteralNode>(start, lvalue * rvalue, common);
            case OperatorType::DIVIDE:
                return make_unique<RealLiteralNode>(start, lvalue / rvalue, common);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to real values.");
        }
    } else if (common->isString()) {
        string lvalue = foldString(start, end, lhs);
        string rvalue = foldString(start, end, rhs);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<StringLiteralNode>(start, lvalue + rvalue, common);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to string values.");
        }
    }
    logger_.error(start, "invalid binary expression.");
    return nullptr;
}

bool
Sema::assertEqual(Ident *aIdent, Ident *bIdent) const {
    if (!aIdent || !bIdent) {
        return false;
    }
    if (aIdent->name() == bIdent->name()) {
        if (aIdent->isQualified() == bIdent->isQualified()) {
            if (aIdent->isQualified()) {
                return dynamic_cast<QualIdent *>(aIdent)->qualifier() ==
                       dynamic_cast<QualIdent *>(bIdent)->qualifier();
            }
            return true;
        }
        return false;
    }
    return false;
}

void
Sema::assertUnique(IdentDef *ident, DeclarationNode *node) {
    if (symbols_->isDuplicate(ident->name())) {
        logger_.error(ident->start(), "duplicate definition: " + ident->name() + ".");
    }
    if (symbols_->isGlobal(ident->name())) {
        logger_.error(ident->start(), "predefined identifier: " + ident->name() + ".");
    }
    symbols_->insert(ident->name(), node);
}

void
Sema::checkExport(DeclarationNode *node) {
    if (node->getLevel() != SymbolTable::MODULE_LEVEL && node->getIdentifier()->isExported()) {
        logger_.error(node->getIdentifier()->start(), "only top-level declarations can be exported.");
    }
}

bool
Sema::assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual, bool var, bool isPtr) {
    if (!expected || !actual) {
        logger_.error(pos, "type mismatch.");
        return false;
    }
    if (expected == actual) {
        return true;
    }
    // Check `ANYTYPE`
    if (expected->kind() == TypeKind::ANYTYPE || expected->kind() == TypeKind::ANYTYPE) {
        return true;
    }
    // Check declared types
    auto expectedId = expected->getIdentifier();
    auto actualId = actual->getIdentifier();
    if (assertEqual(expectedId, actualId)) {
        // the two types are the same type
        return true;
    }
    // Check numeric types
    if (expected->isNumeric() && actual->isNumeric()) {
        if (var) {
            if (expected->kind() == TypeKind::ENTIRE && actual->isInteger()) {
                return true;
            }
            if (expected->kind() != actual->kind()) {
                logger_.error(pos, "type mismatch: cannot pass " + to_string(*actualId) +
                                   " to " + to_string(*expectedId) + " by reference.");
                return false;
            }
            return true;
        }
        if ((expected->isInteger() && actual->isInteger()) || expected->isReal()) {
            if (expected->getSize() < actual->getSize()) {
                logger_.error(pos, "type mismatch: converting " + to_string(*actualId) +
                                   " to " + to_string(*expectedId) + " may lose data.");
                return false;
            }
            return true;
        }
    }
    // Check array type
    if (expected->isArray() && actual->isArray()) {
        auto exp_array = dynamic_cast<ArrayTypeNode *>(expected);
        auto act_array = dynamic_cast<ArrayTypeNode *>(actual);
        if ((exp_array->isOpen() || exp_array->getDimension() == act_array->getDimension())) {
            return assertCompatible(pos, exp_array->getMemberType(), act_array->getMemberType(), var);
        }
    }
    // Check pointer type
    if (expected->isPointer()) {
        if (actual->isPointer()) {
            auto exp_ptr = dynamic_cast<PointerTypeNode *>(expected);
            auto act_ptr = dynamic_cast<PointerTypeNode *>(actual);
            return assertCompatible(pos, exp_ptr->getBase(), act_ptr->getBase(), var, true);
        } else if (actual->kind() == TypeKind::NILTYPE) {
            return true;
        }
    }
    // error logging
    logger_.error(pos, "type mismatch: expected " + format(expected, isPtr) + ", found " + format(actual, isPtr) + ".");
    return false;
}

TypeNode *
Sema::commonType(TypeNode *lhsType, TypeNode *rhsType) const {
    if (lhsType == rhsType) {
        return lhsType;
    } else if (assertEqual(lhsType->getIdentifier(), rhsType->getIdentifier())) {
        return lhsType;
    } else if (lhsType->isNumeric() && rhsType->isNumeric()) {
        if (lhsType->getSize() == rhsType->getSize()) {
            return lhsType->isReal() ? lhsType : rhsType;
        } else {
            return lhsType->getSize() > rhsType->getSize() ? lhsType : rhsType;
        }
    } else if (lhsType->kind() == TypeKind::NILTYPE) {
        return rhsType;
    } else if (rhsType->kind() == TypeKind::NILTYPE) {
        return lhsType;
    }
    return nullptr;
}

string
Sema::format(const TypeNode *type, bool isPtr) const {
    auto name = isPtr ? string("POINTER TO ") : string();
    if (type->getIdentifier()) {
        name += to_string(*type->getIdentifier());
    } else {
        std::stringstream stream;
        stream << *type;
        name += stream.str();
    }
    return name;
}

void Sema::cast(ExpressionNode *expr, TypeNode *expected) {
    if (expr->getType() != expected) {
        expr->setCast(expected);
    }
}

ExpressionNode
*Sema::resolveReference(ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const QualifiedExpression *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return constant->getValue();
        }
    }
    return expr;
}
