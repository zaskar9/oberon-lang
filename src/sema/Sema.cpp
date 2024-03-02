//
// Created by Michael Grossniklaus on 12/17/23.
//

#include "Sema.h"

#include <bitset>
#include <memory>
#include <set>
#include <string>

using std::bitset;
using std::make_unique;
using std::unique_ptr;
using std::set;
using std::string;

Sema::Sema(CompilerConfig &config, ASTContext *context, OberonSystem *system) :
        config_(config), context_(context), system_(system), logger_(config_.logger()), forwards_(), module_(), procs_(),
        symbols_(system_->getSymbolTable()), importer_(config_, context), exporter_(config_, context) {
    boolTy_ = symbols_->getBasicType(to_string(TypeKind::BOOLEAN));
    byteTy = symbols_->getBasicType(to_string(TypeKind::BYTE));
    charTy = symbols_->getBasicType(to_string(TypeKind::CHAR));
    integerTy_ = symbols_->getBasicType(to_string(TypeKind::INTEGER));
    longIntTy_ = symbols_->getBasicType(to_string(TypeKind::LONGINT));
    realTy_ = symbols_->getBasicType(to_string(TypeKind::REAL));
    longRealTy_ = symbols_->getBasicType(to_string(TypeKind::LONGREAL));
    stringTy_ = symbols_->getBasicType(to_string(TypeKind::STRING));
    setTy_ = symbols_->getBasicType(to_string(TypeKind::SET));
    nullTy_ = system_->getBasicType(TypeKind::NOTYPE);
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

void
Sema::onBlockStart() {
    // O07.6.4: If a type P is defined as POINTER TO T, the identifier T can be declared textually
    // following the declaration of P, but, if so, it must lie within the same scope.
    if (!forwards_.empty()) {
        for (const auto& pair: forwards_) {
            auto type = pair.second;
            logger_.error(type->pos(), "undefined forward reference: " + pair.first + ".");
        }
    }
    forwards_.clear();
    symbols_->openScope();
}

void
Sema::onBlockEnd() {
    symbols_->closeScope();
}

ModuleNode *
Sema::onModuleStart(const FilePos &start, unique_ptr<Ident> ident, vector<unique_ptr<ImportNode>> imports) {
    module_ = make_unique<ModuleNode>(start, std::move(ident), std::move(imports));
    assertUnique(module_->getIdentifier(), module_.get());
    module_->setLevel(symbols_->getLevel());
    onBlockStart();
    return module_.get();
}

unique_ptr<ModuleNode>
Sema::onModuleEnd([[maybe_unused]] const FilePos &end, unique_ptr<Ident> ident) {
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
            if (!type->isRecord()) {
                // O07.6.4: Pointer base type must be a record type.
                logger_.error(start, "pointer base type must be a record type.");
            }
            pointer_t->setBase(type);
            forwards_.erase(it);
        }
    } else {
        node->setType(nullTy_);
    }
    return node;
}

ArrayTypeNode *
Sema::onArrayType(const FilePos &start, const FilePos &end,
                  Ident *ident, vector<unique_ptr<ExpressionNode>> indices, TypeNode *type) {
    vector<unsigned> values(indices.size(), 0);
    for (size_t i = 0; i < indices.size(); ++i) {
        auto expr = indices[i].get();
        if (expr) {
            if (!expr->isConstant()) {
                logger_.error(expr->pos(), "constant expression expected.");
            }
            if (expr->getType()) {
                if (expr->isLiteral()) {
                    if (expr->getType()->isInteger()) {
                        auto literal = dynamic_cast<const IntegerLiteralNode *>(expr);
                        if (literal->value() <= 0) {
                            logger_.error(expr->pos(), "array dimension must be a positive value.");
                        }
                        values[i] = (unsigned) literal->value();
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
    }
    if (!type) {
        logger_.error(start, "undefined member type.");
        type = nullTy_;
    }
    vector<unsigned> lengths;
    vector<TypeNode *> types;
    if (type->isArray()) {
        auto arrayTy = dynamic_cast<ArrayTypeNode *>(type);
        lengths.insert(lengths.begin(), arrayTy->lengths().begin(), arrayTy->lengths().end());
        types.insert(types.begin(), arrayTy->types().begin(), arrayTy->types().end());
        logger_.warning(start, "nested array found, use multi-dimensional array instead.");
    }
    for (size_t i = values.size(); i > 0; --i) {
        lengths.insert(lengths.begin(), values[i - 1]);
        types.insert(types.begin(), type);
        type = context_->getOrInsertArrayType(start, end, (i == 1 ? ident : nullptr), lengths.size(), lengths, types);
    }
    return dynamic_cast<ArrayTypeNode *>(type);
}

PointerTypeNode *
Sema::onPointerType(const FilePos &start, const FilePos &end, Ident *ident, unique_ptr<QualIdent> reference) {
    auto sym = symbols_->lookup(reference.get());
    if (!sym) {
        auto node = context_->getOrInsertPointerType(start, end, ident, nullptr);
        forwards_[reference->name()] = node;
        logger_.debug("Found possible forward type reference: " + to_string(*reference));
        return node;
    } else {
        auto type = onTypeReference(start, end, std::move(reference));
        return onPointerType(start, end, ident, type);
    }
}

PointerTypeNode *
Sema::onPointerType([[maybe_unused]] const FilePos &start, const FilePos &end,
                                     Ident *ident, TypeNode *base) {
    if (!base->isRecord()) {
        // O07.6.4: Pointer base type must be a record type.
        logger_.error(start, "pointer base type must be a record type.");
    }
    return context_->getOrInsertPointerType(start, end, ident, base);
}

ProcedureTypeNode *
Sema::onProcedureType([[maybe_unused]] const FilePos &start, const FilePos &end,
                      Ident *ident, vector<unique_ptr<ParameterNode>> params, bool varargs, TypeNode *ret) {
    return context_->getOrInsertProcedureType(start, end, ident, std::move(params), varargs, ret);
}

unique_ptr<ParameterNode>
Sema::onParameter(const FilePos &start, [[maybe_unused]] const FilePos &end,
                  unique_ptr<Ident> ident, TypeNode *type, bool is_var, unsigned index) {
    if (!type) {
        logger_.error(start, "undefined parameter type.");
        type = nullTy_;
    }
    auto node = make_unique<ParameterNode>(start, std::move(ident), type, is_var, index);
    assertUnique(node->getIdentifier(), node.get());
    node->setLevel(symbols_->getLevel());
    return node;
}

RecordTypeNode *
Sema::onRecordType(const FilePos &start, const FilePos &end,
                  Ident *ident, vector<unique_ptr<FieldNode>> fields) {
    if (fields.empty()) {
        logger_.error(start, "records needs at least one field.");
    }
    auto node = context_->getOrInsertRecordType(start, end, ident, std::move(fields));
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
        type = nullTy_;
    }
    return make_unique<FieldNode>(start, std::move(ident), type, index);
}

TypeNode *
Sema::onTypeReference(const FilePos &start, const FilePos &end,
                      unique_ptr<QualIdent> ident, unsigned dimensions) {
    auto sym = symbols_->lookup(ident.get());
    if (sym) {
        TypeNode *type;
        if (sym->getNodeType() == NodeType::array_type ||
            sym->getNodeType() == NodeType::basic_type ||
            sym->getNodeType() == NodeType::record_type ||
            sym->getNodeType() == NodeType::pointer_type) {
            type = dynamic_cast<TypeNode *>(sym);
        } else if (sym->getNodeType() == NodeType::type) {
            type = dynamic_cast<TypeDeclarationNode *>(sym)->getType();
        } else {
            logger_.error(start, to_string(*ident) + " is not a type.");
        }
        if (dimensions == 0) {
            return type;
        }
        vector<unsigned> lengths;
        vector<TypeNode *> types;
        for (size_t i = dimensions; i > 0; --i) {
            lengths.insert(lengths.begin(), 0);
            types.insert(types.begin(), type);
            type = context_->getOrInsertArrayType(start, end, nullptr, lengths.size(), lengths, types);
        }
        return type;
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
        type = nullTy_;
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
    auto ret = proc->getType()->getReturnType();
    if (ret && (ret->isArray() || ret->isRecord())) {
        // O07.10.1: The result type of a procedure can be neither a record nor an array.
        logger_.error(proc->pos(), "result type of a procedure can neither be a record nor an array.");
    }
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
    string err;
    if (!assertAssignable(lvalue.get(), err)) {
        logger_.error(lvalue->pos(), "cannot assign to " + err + ".");
    }
    if (!rvalue) {
        logger_.error(start, "undefined right-hand side in assignment.");
    }
    if (lvalue && rvalue) {
        // Type inference: check that types are compatible
        if (assertCompatible(rvalue->pos(), lvalue->getType(), rvalue->getType())) {
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
        return nullptr;
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
    vector<unique_ptr<Selector>> selectors;
    FilePos v_start = var->start();
    FilePos v_end = var->end();
    auto counter = onQualifiedExpression(v_start, v_end, std::move(var), std::move(selectors));
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
        auto type = onSelectors(ident->start(), ident->end(), sym->getType(), selectors);
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
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, nullTy_);
    }
    // check variable or parameter reference
    if (sym->getNodeType() == NodeType::variable || sym->getNodeType() == NodeType::parameter) {
        auto type = onSelectors(ident->start(), ident->end(), sym->getType(), selectors);
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, type);
    }
    // check procedure reference
    if (sym->getNodeType() == NodeType::procedure) {
        auto proc = dynamic_cast<ProcedureNode *>(sym);
        if (ident->isQualified() && proc->isExtern()) {
            // a fully-qualified external reference needs to be added to module for code generation
            context_->addExternalProcedure(proc);
        }
        auto type = onSelectors(ident->start(), ident->end(), sym->getType(), selectors);
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, type);
    }
    logger_.error(ident->start(), "variable, parameter, or function call expected.");
    return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, nullTy_);;
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

TypeNode *
Sema::onSelectors(const FilePos &start, const FilePos &end,
                  TypeNode *base, vector<unique_ptr<Selector>> &selectors) {
    auto it = selectors.begin();
    it = handleMissingParameters(start, end, base, selectors, it);
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
        it = handleMissingParameters(start, end, base, selectors, it);
        ++it;
    }
    return base;
}

bool
Sema::assertAssignable(const ExpressionNode * expr, string &err) const {
    if (expr->isLiteral()) {
        err = "a constant value";
        return false;
    } else if (expr->getNodeType() == NodeType::qualified_expression) {
        auto decl = dynamic_cast<const QualifiedExpression *>(expr)->dereference();
        if (decl->getNodeType() == NodeType::parameter) {
            auto type = decl->getType();
            if (type->isStructured()) {
                auto param = dynamic_cast<const ParameterNode *>(decl);
                err = "a non-variable structured parameter";
                return param->isVar();
            }
        } else if (decl->getNodeType() == NodeType::constant) {
            err = "a constant";
            return false;
        }
        err = "";
        return true;
    } else {
        err = "an expression";
        return false;
    }
}

Sema::SelectorIterator &
Sema::handleMissingParameters(const FilePos &start, [[maybe_unused]] const FilePos &end,
                              TypeNode *base, Sema::Selectors &selectors, Sema::SelectorIterator &it) {
    if (base && base->isProcedure()) {
        bool found = false;
        if (selectors.empty()) {
            it = selectors.insert(selectors.begin(), make_unique<ActualParameters>());
            found = true;
        } else if (it + 1 != selectors.end() && (*(it +1))->getNodeType() != NodeType::parameter) {
            it = selectors.insert(it, make_unique<ActualParameters>());
            found = true;
        }
        auto proc = dynamic_cast<ProcedureTypeNode *>(base);
        if (found && proc->getReturnType()) {
            logger_.error(start, "function procedures must be called with a parameter list.");
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
        if (!expr) {
            continue;
        }
        if (cnt < proc->parameters().size()) {
            auto param = proc->parameters()[cnt].get();
            if (assertCompatible(expr->pos(), param->getType(), expr->getType(), param->isVar())) {
                if (param->isVar()) {
                    string err;
                    if (!assertAssignable(expr, err)) {
                        logger_.error(expr->pos(), "illegal actual parameter: cannot pass " + err + " by reference.");
                    }
                } else {
                    cast(expr, param->getType());
                }
            }
        } else if (!proc->hasVarArgs()) {
            logger_.error(sel->pos(), "more actual than formal parameters.");
            break;
        }
    }
    return proc->getReturnType();
}

TypeNode *Sema::onArrayIndex(TypeNode *base, ArrayIndex *sel) {
    if (!base->isArray()) {
        logger_.error(sel->pos(), "array " + to_string(base) + " is not an array array.");
        return nullptr;
    }
    auto array = dynamic_cast<ArrayTypeNode *>(base);
    if (sel->indices().size() > array->lengths().size()) {
        logger_.error(sel->pos(), "more indices than array dimensions: " + to_string(sel->indices().size())
                                  + " > " + to_string(array->lengths().size()) + ".");
    }
    auto num = std::min(array->lengths().size(), sel->indices().size());
    for (size_t i = 0; i < num; ++i) {
        auto index = sel->indices()[i].get();
        auto type = index->getType();
        if (type->kind() == TypeKind::INTEGER) {
            if (index->isLiteral()) {
                auto literal = dynamic_cast<const IntegerLiteralNode *>(index);
                if (array->isOpen()) {
                    if (literal->value() < 0) {
                        logger_.error(literal->pos(), "negative value " + to_string(literal->value())
                                                      + " is not a valid array index.");
                    }
                } else {
                    long length = (long) array->lengths()[i];
                    assertInBounds(literal, 0, length - 1);
                }
            }
        } else {
            logger_.error(sel->pos(), "integer expression expected.");
        }
    }
    return array->types()[num - 1];
}

TypeNode *Sema::onDereference(TypeNode *base, Dereference *sel) {
    if (!base->isPointer()) {
        logger_.error(sel->pos(), "pointer " + to_string(base) + " is not a pointer pointer.");
        return nullptr;
    }
    auto pointer = dynamic_cast<PointerTypeNode *>(base);
    return pointer->getBase();
}

TypeNode *Sema::onRecordField(TypeNode *base, RecordField *sel) {
    if (!base->isRecord()) {
        logger_.error(sel->pos(), "record " + to_string(base) + " is not a record record.");
        return nullptr;
    }
    auto record = dynamic_cast<RecordTypeNode *>(base);
    auto ref = dynamic_cast<RecordField *>(sel);
    auto field = record->getField(ref->ident()->name());
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
        type = nullTy_;
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
    TypeNode *common = nullptr;
    TypeNode *result = nullptr;
    switch (op) {
        case OperatorType::EQ:
        case OperatorType::NEQ:
        case OperatorType::LT:
        case OperatorType::LEQ:
        case OperatorType::GT:
        case OperatorType::GEQ:
            common = commonType(start, lhsType, rhsType);
            result = boolTy_;
            break;
        case OperatorType::IN:
            common = nullptr;
            result = boolTy_;
            if (!lhsType->isInteger()) {
                logger_.error(lhs->pos(), "integer expression expected.");
            }
            if (!rhsType->isSet()) {
                logger_.error(rhs->pos(), "set expression expected");
            }
            break;
        case OperatorType::PLUS:
        case OperatorType::MINUS:
        case OperatorType::TIMES:
            common = commonType(start, lhsType, rhsType);
            result = common;
            break;
        case OperatorType::DIV:
        case OperatorType::MOD:
            if (!lhsType->isInteger() || !rhsType->isInteger()) {
                logger_.error(start, "integer division requires integer arguments.");
            }
            common = commonType(start, lhsType, rhsType);
            result = common;
            break;
        case OperatorType::DIVIDE:
            common = commonType(start, lhsType, rhsType);
            result = common;
            if (common->isInteger()) {
                common = this->realTy_;
                result = common;
            }
            break;
        case OperatorType::OR:
        case OperatorType::AND:
            if (!lhsType->isBoolean() || !rhsType->isBoolean()) {
                logger_.error(start, "operator " + to_string(op) + " requires boolean arguments.");
            }
            common = boolTy_;
            result = common;
            break;
        default:
            logger_.error(start, "unsupported operator: " + to_string(op) + ".");
    }
    if (!result) {
        logger_.error(start, "could not infer result type of expression.");
    }
    // Folding
    if (lhs->isConstant() && rhs->isConstant()) {
        return fold(start, end, op, lhs.get(), rhs.get(), result);
    }
    if (common) {
        // Casting left-hand side to common type
        cast(lhs.get(), common);
        // Casting right-hand side to common type
        cast(rhs.get(), common);
    }
    return make_unique<BinaryExpressionNode>(start, op, std::move(lhs), std::move(rhs), result);
}

unique_ptr<ExpressionNode>
Sema::onRangeExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                        unique_ptr<ExpressionNode> lower, unique_ptr<ExpressionNode> upper) {
    if (!lower) {
        logger_.error(start, "undefined lower bound in range expression.");
        return nullptr;
    }
    auto loType = lower->getType();
    if (!loType->isInteger()) {
        logger_.error(lower->pos(), "integer expression expected.");
    }
    long loValue = -1;
    if (lower->isLiteral()) {
        loValue = assertInBounds(dynamic_cast<const IntegerLiteralNode *>(lower.get()), 0, 31);
    }
    if (!upper) {
        logger_.error(start, "undefined upper bound in range expression.");
        return nullptr;
    }
    auto upType = upper->getType();
    if (!upType->isInteger()) {
        logger_.error(upper->pos(), "integer expression expected.");
    }
    long upValue = -1;
    if (upper->isLiteral()) {
        upValue = assertInBounds(dynamic_cast<const IntegerLiteralNode *>(upper.get()), 0, 31);
    }
    auto common = loType;
    if (loType->getSize() > upType->getSize()) {
        upper->setCast(loType);
    } else if (loType->getSize() < upType->getSize()) {
        lower->setCast(upType);
        common = upType;
    }
    if (loValue >= 0 && upValue >= 0) {
        if (loValue >= upValue) {
            logger_.error(start, "lower bound must be smaller than upper bound.");
        }
        bitset<32> result;
        result.flip();
        result >>= std::size_t(loValue);
        result <<= std::size_t(31 - upValue + loValue);
        result >>= std::size_t(31 - upValue);
        return make_unique<RangeLiteralNode>(start, result, loValue, upValue, common);
    }
    return make_unique<RangeExpressionNode>(start, std::move(lower), std::move(upper), common);
}

unique_ptr<ExpressionNode> Sema::onSetExpression(const FilePos &start, [[maybe_unused]] const FilePos &end,
                                                 vector<unique_ptr<ExpressionNode>> elements) {
    long last = -1;
    for (auto &elem : elements) {
        if (!elem->getType()->isInteger()) {
            logger_.error(elem->pos(), "set expression expected.");
        } else if (elem->getNodeType() == NodeType::range_expression) {
            auto range = dynamic_cast<RangeExpressionNode *>(elem.get());
            if (range->getLower()->isLiteral()) {
                long value = dynamic_cast<const IntegerLiteralNode *>(range->getLower())->value();
                if (value <= last) {
                    logger_.error(range->getLower()->pos(), "element must be larger than previous element.");
                }
                last = value;
            }
            if (range->getUpper()->isLiteral()) {
                last = dynamic_cast<const IntegerLiteralNode *>(range->getUpper())->value();
            }
        } else if (elem->getNodeType() == NodeType::range) {
            auto range = dynamic_cast<RangeLiteralNode *>(elem.get());
            if (last >= range->lower()) {
                logger_.error(range->pos(), "element must be larger than previous element.");
            }
            last = range->upper();
        } else {
            if (elem->isLiteral()) {
                auto value = assertInBounds(dynamic_cast<const IntegerLiteralNode *>(elem.get()), 0, 31);
                if (value <= last) {
                    logger_.error(elem->pos(), "element must be larger than previous element.");
                }
                last = value;
            }
        }
    }
    auto expr = make_unique<SetExpressionNode>(start, std::move(elements), setTy_);
    if (expr->isConstant()) {
        bitset<32> result;
        for (auto &elem : expr->elements()) {
            if (elem->getNodeType() == NodeType::range) {
                auto range = dynamic_cast<const RangeLiteralNode *>(elem.get());
                result |= range->value();
            } else {
                long pos = foldInteger(start, end, elem.get());
                result.set(std::size_t(pos));
            }
        }
        return make_unique<SetLiteralNode>(start, result, setTy_);
    }
    return expr;
}

long Sema::assertInBounds(const IntegerLiteralNode *literal, long lower, long upper) {
    long value = literal->value();
    if (value < lower || value > upper) {
        logger_.error(literal->pos(), "value " + to_string(value) + " out of bounds [" +
                                      to_string(lower) + ".." + to_string(upper) + "].");
        return lower;
    }
    return value;
}

unique_ptr<BooleanLiteralNode>
Sema::onBooleanLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, bool value) {
    return make_unique<BooleanLiteralNode>(start, value, boolTy_);
}

unique_ptr<IntegerLiteralNode>
Sema::onIntegerLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, long value, bool ext) {
    TypeNode *type = ext ? longIntTy_ : integerTy_;
    return make_unique<IntegerLiteralNode>(start, value, type);
}

unique_ptr<RealLiteralNode>
Sema::onRealLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, double value, bool ext) {
    TypeNode *type = ext ? longRealTy_ : realTy_;
    return make_unique<RealLiteralNode>(start, value, type);
}

unique_ptr<StringLiteralNode>
Sema::onStringLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, const string &value) {
    return make_unique<StringLiteralNode>(start, value, stringTy_);
}

unique_ptr<NilLiteralNode>
Sema::onNilLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end) {
    return make_unique<NilLiteralNode>(start, symbols_->getNilType());
}

unique_ptr<SetLiteralNode>
Sema::onSetLiteral(const FilePos &start, [[maybe_unused]] const FilePos &end, bitset<32> value) {
    return make_unique<SetLiteralNode>(start, value, setTy_);
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
    if (expr->getNodeType() == NodeType::boolean) {
        return dynamic_cast<const BooleanLiteralNode *>(expr)->value();
    }
    logger_.error(start, "expression is not a constant boolean value.");
    return {};
}

long
Sema::foldInteger(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::integer) {
        return dynamic_cast<const IntegerLiteralNode *>(expr)->value();
    }
    logger_.error(start, "expression is not a constant integer value.");
    return {};
}

double
Sema::foldReal(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::real) {
        return dynamic_cast<const RealLiteralNode *>(expr)->value();
    } else if (expr->getNodeType() == NodeType::integer) {
        // promote integer to real
        return (double) foldInteger(start, end, expr);
    }
    logger_.error(start, "expression is not a constant real value.");
    return {};
}

string
Sema::foldString(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::string) {
        return dynamic_cast<const StringLiteralNode *>(expr)->value();
    }
    logger_.error(start, "expression is not a constant string value.");
    return {};
}

bitset<32>
Sema::foldSet(const FilePos &start, [[maybe_unused]] const FilePos &end, ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::set) {
        return dynamic_cast<const SetLiteralNode *>(expr)->value();
    }
    logger_.error(start, "expression is not a constant set value.");
    return {};
}

std::unique_ptr<LiteralNode>
Sema::fold(const FilePos &start, const FilePos &end, ExpressionNode *expr) {
    auto type = expr->getType();
    if (type) {
        auto cast = expr->getCast();
        if (type->kind() == TypeKind::INTEGER || type->kind() == TypeKind::LONGINT) {
            return make_unique<IntegerLiteralNode>(expr->pos(), foldInteger(start, end, expr), type, cast);
        } else if (type->kind() == TypeKind::REAL || type->kind() == TypeKind::LONGREAL) {
            return make_unique<RealLiteralNode>(expr->pos(), foldReal(start, end, expr), type, cast);
        } else if (type->kind() == TypeKind::BOOLEAN) {
            return make_unique<BooleanLiteralNode>(expr->pos(), foldBoolean(start, end, expr), type, cast);
        } else if (type->kind() == TypeKind::STRING) {
            return make_unique<StringLiteralNode>(expr->pos(), foldString(start, end, expr), type, cast);
        } else if (type->kind() == TypeKind::SET) {
            return make_unique<SetLiteralNode>(expr->pos(), foldSet(start, end, expr), type, cast);
        }
        logger_.error(start, "unsupported literal type: " + to_string(*type->getIdentifier()) + ".");
    } else {
        logger_.error(start, "unsupported literal type.");
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
            case OperatorType::NEG: {
                // negating an integer literal can change its type from LONGINT to INTEGER
                auto literal = make_unique<IntegerLiteralNode>(start, -value, type, cast);
                literal->setType(literal->isLong() ? longIntTy_ : integerTy_);
                return literal;
            }
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
    } else if (type->isSet()) {
        auto value = foldSet(start, end, expr);
        switch (op) {
            case OperatorType::NEG:
                return make_unique<SetLiteralNode>(start, value.flip(), type, cast);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to set values.");
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
                case OperatorType::EQ:
                    return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                case OperatorType::NEQ:
                    return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
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
        } else if (rhs->getType()->isSet()) {
            auto rvalue = foldSet(start, end, rhs);
            if (lhs->getType()->isSet()) {
                auto lvalue = foldSet(start, end, lhs);
                switch (op) {
                    case OperatorType::EQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue == rvalue, common);
                    case OperatorType::NEQ:
                        return make_unique<BooleanLiteralNode>(start, lvalue != rvalue, common);
                    case OperatorType::LEQ:
                        return make_unique<BooleanLiteralNode>(start, (lvalue & rvalue.flip()).none(), common);
                    case OperatorType::GEQ:
                        return make_unique<BooleanLiteralNode>(start, (rvalue & lvalue.flip()).none(), common);
                    default:
                        logger_.error(start, "operator " + to_string(op) + " cannot be applied to set values.");
                }
            } else if (op == OperatorType::IN && lhs->getType()->isInteger()) {
                long lvalue = assertInBounds(dynamic_cast<IntegerLiteralNode *>(lhs), 0, 31);
                return make_unique<BooleanLiteralNode>(start, rvalue.test(std::size_t(lvalue)), common);
            }
            logger_.error(start, "operator " + to_string(op) + " cannot be applied here.");
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
    } else if (common->isSet()) {
        auto lvalue = foldSet(start, end, lhs);
        auto rvalue = foldSet(start, end, rhs);
        switch (op) {
            case OperatorType::PLUS:
                return make_unique<SetLiteralNode>(start, lvalue | rvalue, common);
            case OperatorType::MINUS:
                return make_unique<SetLiteralNode>(start, lvalue & rvalue.flip(), common);
            case OperatorType::TIMES:
                return make_unique<SetLiteralNode>(start, lvalue & rvalue, common);
            case OperatorType::DIVIDE:
                return make_unique<SetLiteralNode>(start, lvalue ^ rvalue, common);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to set values.");
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
    if (expected->kind() == TypeKind::ANYTYPE) {
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
        if ((expected->isInteger() && actual->isInteger())
            || (expected->isReal() && actual->isReal())) {
            if (expected->getSize() < actual->getSize()) {
                logger_.error(pos, "type mismatch: converting from " + to_string(*actualId) +
                                   " to " + to_string(*expectedId) + " may lose data.");
                return false;
            }
            return true;
        }
        if (expected->isReal() && actual->isInteger()) {
            return true;
        }
    }
    // Check array type
    if (expected->isArray() && actual->isArray()) {
        auto exp_array = dynamic_cast<ArrayTypeNode *>(expected);
        auto act_array = dynamic_cast<ArrayTypeNode *>(actual);
        // Check `ANYTYPE`
        if (exp_array->getMemberType()->kind() == TypeKind::ANYTYPE) {
            return true;
        }
        if (exp_array->dimensions() == act_array->dimensions()) {
            if (exp_array->isOpen()) {
                return assertCompatible(pos, exp_array->getMemberType(), act_array->getMemberType(), var);
            } else {
                for (size_t i = 0; i < exp_array->dimensions(); ++i) {
                    if (!assertCompatible(pos, exp_array->types()[i], act_array->types()[i], var)) {
                        return false;
                    }
                }
                return true;
            }
        } else {
            logger_.error(pos, "type mismatch: incompatible array dimensions, expected " +
                    to_string(exp_array->dimensions()) + ", found " + to_string(act_array->dimensions()) + ".");
            return false;
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
    // Error logging
    logger_.error(pos, "type mismatch: expected " + format(expected, isPtr) + ", found " + format(actual, isPtr) + ".");
    return false;
}

TypeNode *
Sema::commonType(const FilePos &pos, TypeNode *lhsType, TypeNode *rhsType) const {
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
    logger_.error(pos, "incompatible types (" + to_string(*lhsType->getIdentifier()) + ", " +
                       to_string(*rhsType->getIdentifier()) + ")");
    return nullptr;
}

string
Sema::format(const TypeNode *type, bool isPtr) {
    std::stringstream stream;
    if (isPtr) {
        stream << "POINTER TO ";
    }
    if (type->isArray()) {
        auto array = dynamic_cast<const ArrayTypeNode *>(type);
        stream << "ARRAY ";
        string sep;
        for (unsigned length: array->lengths()) {
            stream << sep << length;
            sep = ", ";
        }
        stream << " OF " << *array->getMemberType();
    } else {
        stream << *type;
    }
    return stream.str();
}

void Sema::cast(ExpressionNode *expr, TypeNode *expected) {
    if (expr->getType() != expected) {
        expr->setCast(expected);
    }
}
