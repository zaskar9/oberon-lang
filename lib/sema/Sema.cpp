//
// Created by Michael Grossniklaus on 12/17/23.
//

#include "Sema.h"

#include <bitset>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>

using std::bitset;
using std::make_unique;
using std::unique_ptr;
using std::unordered_set;
using std::set;
using std::string;

Sema::Sema(CompilerConfig &config, ASTContext *context, OberonSystem *system) :
        config_(config), context_(context), system_(system), logger_(config_.logger()),
        forwards_(), procs_(), caseTys_(), loops_(), symbols_(system_->getSymbolTable()),
        importer_(config_, context, symbols_), exporter_(config_, context) {
    boolTy_ = system_->getBasicType(TypeKind::BOOLEAN);
    byteTy_ = system_->getBasicType(TypeKind::BYTE);
    charTy_ = system_->getBasicType(TypeKind::CHAR);
    shortIntTy_ = system->getBasicType(TypeKind::SHORTINT);
    integerTy_ = system_->getBasicType(TypeKind::INTEGER);
    longIntTy_ = system_->getBasicType(TypeKind::LONGINT);
    realTy_ = system_->getBasicType(TypeKind::REAL);
    longRealTy_ = system_->getBasicType(TypeKind::LONGREAL);
    stringTy_ = system_->getBasicType(TypeKind::STRING);
    setTy_ = system_->getBasicType(TypeKind::SET);
    anyTy_ = system->getBasicType(TypeKind::ANYTYPE);
    noTy_ = system_->getBasicType(TypeKind::NOTYPE);
    typeTy_ = system_->getBasicType(TypeKind::TYPE);
}

void
Sema::onTranslationUnitStart(const FilePos &start, const FilePos &, const unique_ptr<Ident> &ident) const {
    const string name = ident->name();
    if (symbols_->getModule(name)) {
        logger_.error(start, "duplicate module definition: " + name + ".");
    }
    symbols_->addModule(name, true);
}

void
Sema::onTranslationUnitEnd(const string &name) {
#ifdef _DEBUG
    if (logger_.getErrorCount() == 0) {
#else
    if (logger_.getErrorCount() == 0 && !config_.isJit()) {
#endif
        exporter_.write(name, symbols_);
    }
}

void Sema::onBlockStart() {
    symbols_->openScope();
}

void Sema::onBlockEnd() {
    symbols_->closeScope();
}

void Sema::onDeclarations() {
    // O07.6.4: If a type P is defined as POINTER TO T, the identifier T can be declared textually
    // following the declaration of P, but, if so, it must lie within the same scope.
    while (!forwards_.empty()) {
        auto pair = std::move(forwards_.back());
        forwards_.pop_back();
        auto start = pair.first->start();
        auto end = pair.first->end();
        auto pointer_t = pair.second;
        auto type = onTypeReference(start, end, std::move(pair.first));
        if (type->kind() != TypeKind::NOTYPE) {
            logger_.debug("Resolving forward reference: " + to_string(*type->getIdentifier()));
            if (!type->isRecord()) {
                // O07.6.4: Pointer base type must be a record type.
                logger_.error(start, "pointer base type must be a record type.");
            }
            pointer_t->setBase(type);
        } else {
            logger_.error(start, "undefined forward reference.");
        }
    }
}

unique_ptr<ModuleNode>
Sema::onModuleStart(const FilePos &start, unique_ptr<Ident> ident) {
    auto module = make_unique<ModuleNode>(start, std::move(ident));
    // assertUnique(module->getIdentifier(), module.get());
    module->setScope(symbols_->getLevel());
    onBlockStart();
    return module;
}

void
Sema::onModuleEnd(const FilePos &, const unique_ptr<Ident>& ident) {
    onBlockEnd();
    auto module = context_->getTranslationUnit();
    if (*module->getIdentifier() != *ident.get()) {
        logger_.error(ident->start(), "module name mismatch: expected " + to_string(*module->getIdentifier()) +
                                       ", found " + to_string(*ident) + ".");
    }
}

unique_ptr<ImportNode>
Sema::onImport(const FilePos &start, const FilePos &,
               unique_ptr<Ident> alias, unique_ptr<Ident> ident) {
    auto node = make_unique<ImportNode>(start, std::move(alias), std::move(ident));
    auto name = node->getModule()->name();
    // Check for duplicate imports
    for (const auto &import : context_->getTranslationUnit()->imports()) {
        if (import->getModule()->name() == name) {
            logger_.error(node->pos(), "duplicate import of module " + name + ".");
            break;
        }
    }
    if (name == "SYSTEM") {
        // Import the pseudo-module SYSTEM
        context_->addExternalModule(std::make_unique<ModuleNode>(std::make_unique<Ident>(name)));
    } else if (name == context_->getTranslationUnit()->getIdentifier()->name()) {
        // Check for recursive import
        logger_.error(node->pos(), "module " + name + " must not import itself.");
    } else {
        // Check whether the module can be imported
        if (!importer_.read(name)) {
            logger_.error(node->pos(), "module " + name + " could not be imported.");
        }
    }
    // Set the alias for the module in the symbol table
    if (node->getAlias()) {
        symbols_->addAlias(node->getAlias()->name(), name);
    }
    return node;
}

unique_ptr<ConstantDeclarationNode>
Sema::onConstant(const FilePos &start, const FilePos &,
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
    auto node = make_unique<ConstantDeclarationNode>(start, std::move(ident), std::move(expr), expr ? expr->getType() : noTy_);
    assertUnique(node->getIdentifier(), node.get());
    node->setScope(symbols_->getLevel());
    node->setModule(context_->getTranslationUnit());
    checkExport(node.get());
    return node;
}

unique_ptr<TypeDeclarationNode>
Sema::onType(const FilePos &start, const FilePos &,
             unique_ptr<IdentDef> ident, TypeNode *type) {
    auto node = make_unique<TypeDeclarationNode>(start, std::move(ident), type ? type : noTy_);
    assertUnique(node->getIdentifier(), node.get());
    node->setScope(symbols_->getLevel());
    node->setModule(context_->getTranslationUnit());
    checkExport(node.get());
    return node;
}

ArrayTypeNode *
Sema::onArrayType(const FilePos &start, const FilePos &end, vector<unique_ptr<ExpressionNode>> indices, TypeNode *type) {
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
        type = noTy_;
    }
    vector<unsigned> lengths;
    vector<TypeNode *> types;
    if (type->isArray()) {
        auto array_t = dynamic_cast<ArrayTypeNode *>(type);
        lengths.insert(lengths.begin(), array_t->lengths().begin(), array_t->lengths().end());
        types.insert(types.begin(), array_t->types().begin(), array_t->types().end());
        logger_.warning(start, "nested array found, use multi-dimensional array instead.");
    }
    for (size_t i = values.size(); i > 0; --i) {
        lengths.insert(lengths.begin(), values[i - 1]);
        types.insert(types.begin(), type);
        type = context_->getOrInsertArrayType(start, end, static_cast<unsigned>(lengths.size()), lengths, types);
    }
    auto array_t = dynamic_cast<ArrayTypeNode *>(type);
    array_t->setBase(array_t);
    return array_t;
}

void
Sema::onPointerTypeEnd(const FilePos &, const FilePos &, PointerTypeNode *type, unique_ptr<QualIdent> reference) {
    logger_.debug("Found possible forward type reference: " + to_string(*reference) + ".");
    forwards_.push_back({std::move(reference), type});
}

PointerTypeNode *
Sema::onPointerTypeStart(const FilePos &start, const FilePos &end) const {
    return context_->getOrInsertPointerType(start, end, noTy_);
}


void
Sema::onPointerTypeEnd(const FilePos &start, const FilePos &, PointerTypeNode *type, TypeNode *base) const {
    if (!base->isRecord()) {
        // O07.6.4: Pointer base type must be a record type.
        logger_.error(start, "pointer base type must be a record type.");
    }
    type->setBase(base);
}

ProcedureTypeNode *
Sema::onProcedureType(const FilePos &start, const FilePos &end,
                      vector<unique_ptr<ParameterNode>> params, const bool varargs, TypeNode *ret) const {
    if (ret && (ret->isArray() || ret->isRecord())) {
        // O07.10.1: The result type of a procedure can be neither a record nor an array.
        logger_.error(start, "result type of a procedure can neither be a record nor an array.");
    }
    return context_->getOrInsertProcedureType(start, end, std::move(params), varargs, ret);
}

unique_ptr<ParameterNode>
Sema::onParameter(const FilePos &start, const FilePos &,
                  unique_ptr<Ident> ident, TypeNode *type, bool is_var, unsigned index) {
    if (!type) {
        logger_.error(start, "undefined parameter type.");
        type = noTy_;
    }
    auto node = make_unique<ParameterNode>(start, std::move(ident), type, is_var, index);
    assertUnique(node->getIdentifier(), node.get());
    node->setScope(symbols_->getLevel());
    return node;
}

RecordTypeNode *
Sema::onRecordType(const FilePos &start, const FilePos &end,
                   unique_ptr<QualIdent> ident, vector<unique_ptr<FieldNode>> fields) {
    RecordTypeNode *base = nullptr;
    if (ident) {
        auto sym = symbols_->lookup(ident.get());
        if (sym && sym->getNodeType() == NodeType::type) {
            auto type = dynamic_cast<TypeDeclarationNode *>(sym)->getType();
            if (type->isRecord()) {
                base = dynamic_cast<RecordTypeNode *>(type);
            } else {
                logger_.error(ident->start(), "base type must be a record type.");
            }
        } else {
            logger_.error(start, "undefined type: " + to_string(*ident) + ".");
        }
    }
    auto node = context_->getOrInsertRecordType(start, end, base, std::move(fields));
    set<string> names;
    for (size_t i = 0; i < node->getFieldCount(); i++) {
        auto field = node->getField(i);
        auto name = field->getIdentifier()->name();
        if (base && base->getField(name)) {
            logger_.error(field->pos(), "redefinition of record field: " + to_string(*field->getIdentifier()) + ".");
            continue;
        }
        if (names.count(name)) {
            logger_.error(field->pos(), "duplicate record field: " + to_string(*field->getIdentifier()) + ".");
            continue;
        } else {
            names.insert(name);
        }
    }
    return node;
}

unique_ptr<FieldNode>
Sema::onField(const FilePos &start, const FilePos &,
              unique_ptr<IdentDef> ident, TypeNode *type, unsigned index) {
    if (!type) {
        logger_.error(start, "undefined record field type.");
        type = noTy_;
    }
    return make_unique<FieldNode>(start, std::move(ident), type, index);
}

TypeNode *
Sema::onTypeReference(const FilePos &start, const FilePos &end,
                      const unique_ptr<QualIdent> &ident, const unsigned dimensions) const {
    const auto sym = symbols_->lookup(ident.get());
    if (!sym) {
        logger_.error(start, "undefined type: " + to_string(*ident) + ".");
        return noTy_;
    }
    const auto decl = dynamic_cast<TypeDeclarationNode * >(sym);
    if (!decl) {
        logger_.error(start, to_string(*ident) + " is not a type.");
        return noTy_;
    }
    if (auto type = decl->getType(); type->kind() != TypeKind::NOTYPE) {
        if (dimensions == 0) {
            return type;
        }
        vector<unsigned> lengths;
        vector<TypeNode *> types;
        for (size_t i = dimensions; i > 0; --i) {
            lengths.insert(lengths.begin(), 0);
            types.insert(types.begin(), type);
            type = context_->getOrInsertArrayType(start, end, static_cast<unsigned>(lengths.size()), lengths, types);
        }
        const auto array_t = dynamic_cast<ArrayTypeNode *>(type);
        array_t->setBase(array_t);
        return type;
    }
    logger_.error(start, "undefined type: " + to_string(*ident) + ".");
    return noTy_;
}

unique_ptr<VariableDeclarationNode>
Sema::onVariable(const FilePos &start, const FilePos &,
                 unique_ptr<IdentDef> ident, TypeNode *type, int index) {
    if (!type) {
        logger_.error(start, "undefined variable type.");
        type = noTy_;
    }
    auto node = make_unique<VariableDeclarationNode>(start, std::move(ident), type, index);
    assertUnique(node->getIdentifier(), node.get());
    node->setScope(symbols_->getLevel());
    node->setModule(context_->getTranslationUnit());
    checkExport(node.get());
    return node;
}

unique_ptr<ProcedureDeclarationNode>
Sema::onProcedureDeclaration(const FilePos &start, const FilePos &, unique_ptr<IdentDef> ident,
                             ProcedureTypeNode *type, const string &cnv, string &name) {
    auto convention = CallingConvention::OLANG;
    if (cnv.empty()) {
        logger_.warning(start, "no calling convention specified, using default calling convention.");
    } else if (cnv == "C") {
        convention = CallingConvention::C;
    } else if (cnv != "OLANG") {
        logger_.error(start, "unsupported calling convention: " + cnv + ".");
    }
    if (name.empty()) {
        name = ident->name();
        logger_.warning(start, "no external procedure name specified, defaulting to: " + name + ".");
    }
    if (ident->isExported()) {
        logger_.error(start, "cannot export external procedures.");
    }
    auto proc = make_unique<ProcedureDeclarationNode>(start, std::move(ident), type, convention, name);
    assertUnique(proc->getIdentifier(), proc.get());
    proc->setScope(symbols_->getLevel());
    proc->setModule(context_->getTranslationUnit());
    checkExport(proc.get());
    if (proc->getScope() != SymbolTable::MODULE_SCOPE) {
        logger_.error(proc->pos(), "only top-level procedures can be external.");
    }
    return proc;
}

ProcedureDefinitionNode *
Sema::onProcedureDefinitionStart(const FilePos &start, unique_ptr<IdentDef> ident) {
    procs_.push(make_unique<ProcedureDefinitionNode>(start, std::move(ident)));
    const auto proc = procs_.top().get();
    assertUnique(proc->getIdentifier(), proc);
    proc->setScope(symbols_->getLevel());
    proc->setModule(context_->getTranslationUnit());
    checkExport(proc);
    onBlockStart();
    return proc;
}

unique_ptr<ProcedureDefinitionNode>
Sema::onProcedureDefinitionEnd(const FilePos &, const unique_ptr<Ident> &ident) {
    onBlockEnd();
    auto proc = std::move(procs_.top());
    procs_.pop();
    if (proc->getType()->getReturnType() && !proc->statements()->isReturn()) {
        logger_.error(proc->pos(), "not all control flow paths of the procedure return a result.");
    }
    if (*proc->getIdentifier() != *ident) {
        logger_.error(ident->start(), "procedure name mismatch: expected " + to_string(*proc->getIdentifier()) +
                                       ", found " + to_string(*ident) + ".");
    }
    return proc;
}

void Sema::onStatementSequence(const StatementSequenceNode *stmts) const {
    const size_t length = stmts->getStatementCount() - 1;
    const size_t termIdx = stmts->getTerminatorIndex();
    if (stmts->hasTerminator() && termIdx < length) {
        const auto stmt = stmts->getStatement(termIdx + 1);
        logger_.error(stmt->pos(), "unreachable code.");
    }
}

unique_ptr<AssignmentNode>
Sema::onAssignment(const FilePos &start, const FilePos &,
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
            if (lvalue->getType() != rvalue->getType()) {
                if (rvalue->isLiteral()) {
                    castLiteral(rvalue, lvalue->getType());
                } else {
                    // Type inference: compatibility does not check if the `CHAR` value is a literal
                    if (lvalue->getType()->isArray() && rvalue->getType()->isChar()) {
                        logger_.error(lvalue->pos(), "type mismatch: cannot assign a non-constant character value to a string variable.");
                    }
                    cast(rvalue.get(), lvalue->getType());
                }
            }
        }
    }
    return make_unique<AssignmentNode>(start, std::move(lvalue), std::move(rvalue));
}

unique_ptr<IfThenElseNode>
Sema::onIf(const FilePos &start, const FilePos &,
                    unique_ptr<ExpressionNode> condition,
                    unique_ptr<StatementSequenceNode> thenStmts,
                    vector<unique_ptr<ElseIfNode>> elseIfs,
                    unique_ptr<StatementSequenceNode> elseStmts) const {
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
Sema::onElseIf(const FilePos &start, const FilePos &,
               unique_ptr<ExpressionNode> condition,
               unique_ptr<StatementSequenceNode> stmts) const {
    if (!condition) {
        logger_.error(start, "undefined condition in elsif-statement.");
    }
    auto type = condition->getType();
    if (type && type->kind() != TypeKind::BOOLEAN) {
        logger_.error(condition->pos(), "Boolean expression expected.");
    }
    return make_unique<ElseIfNode>(start, std::move(condition), std::move(stmts));
}

void Sema::onLoopStart(const FilePos &start) {
    loops_.push(start);
}

unique_ptr<LoopNode>
Sema::onLoop(const FilePos &start, const FilePos &, unique_ptr<StatementSequenceNode> stmts) {
    if (!stmts->hasExit()) {
        logger_.warning(start, "LOOP statement without EXIT found.");
    }
    loops_.pop();
    return make_unique<LoopNode>(start, std::move(stmts));
}

unique_ptr<WhileLoopNode>
Sema::onWhileLoop(const FilePos &start, const FilePos &,
                  unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts,
                  vector<unique_ptr<ElseIfNode>> elseIfs) {
    if (!condition) {
        logger_.error(start, "undefined condition in while-loop.");
    }
    auto type = condition->getType();
    if (type && type->kind() != TypeKind::BOOLEAN) {
        logger_.error(condition->pos(), "Boolean expression expected.");
    }
    loops_.pop();
    return make_unique<WhileLoopNode>(start, std::move(condition), std::move(stmts), std::move(elseIfs));
}

unique_ptr<RepeatLoopNode>
Sema::onRepeatLoop(const FilePos &start, const FilePos &,
                   unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts) {
    if (!condition) {
        logger_.error(start, "undefined condition in repeat-loop.");
    }
    auto type = condition->getType();
    if (type && type->kind() != TypeKind::BOOLEAN) {
        logger_.error(condition->pos(), "Boolean expression expected.");
    }
    loops_.pop();
    return make_unique<RepeatLoopNode>(start, std::move(condition), std::move(stmts));
}

unique_ptr<ForLoopNode>
Sema::onForLoop(const FilePos &start, const FilePos &,
                unique_ptr<QualIdent> var,
                unique_ptr<ExpressionNode> low, unique_ptr<ExpressionNode> high, unique_ptr<ExpressionNode> step,
                unique_ptr<StatementSequenceNode> stmts) {
    vector<unique_ptr<Selector>> selectors;
    const FilePos v_start = var->start();
    const FilePos v_end = var->end();
    auto counter = onQualifiedExpression(v_start, v_end, std::move(var), std::move(selectors));
    if (!counter) {
        logger_.error(start, "undefined counter variable in for-loop.");
    }
    const auto ident = counter->ident();
    if (ident->isQualified()) {
        logger_.error(ident->start(), to_string(*ident) + " cannot be used as a loop counter.");
    }
    TypeNode *type = nullptr;
    if (counter->getNodeType() == NodeType::qualified_expression) {
        const auto decl = counter->dereference();
        if (decl->getNodeType() != NodeType::variable) {
            logger_.error(ident->start(), "variable expected.");
        }
        type = decl->getType();
        if (type && !type->isInteger()) {
            logger_.error(ident->start(), "type mismatch: integer type expected, found " + format(type) + ".");
        }
    } else {
        logger_.error(ident->start(), to_string(*ident) + " cannot be used as a loop counter.");
    }
    if (low) {
        if (type && assertCompatible(low->pos(), type, low->getType())) {
            cast(low.get(), type);
        }
    } else {
        logger_.error(start, "undefined low value in for-loop.");
    }
    if (high) {
        if (type && assertCompatible(high->pos(), type, high->getType())) {
            cast(high.get(), type);
        }
    } else {
        logger_.error(start, "undefined high value in for-loop.");
    }
    if (step) {
        if (step->isLiteral()) {
            if (type && assertCompatible(step->pos(), type, step->getType())) {
                const auto val = dynamic_cast<IntegerLiteralNode *>(step.get())->value();
                if (val == 0) {
                    logger_.error(step->pos(), "step value cannot be zero.");
                } else {
                    cast(step.get(), integerTy_);
                }
            }
        } else  {
            logger_.error(step->pos(), "constant expression expected.");
        }
    } else {
        step = onIntegerLiteral(EMPTY_POS, EMPTY_POS, 1, TypeKind::INTEGER);
    }
    loops_.pop();
    return make_unique<ForLoopNode>(start, std::move(counter), std::move(low), std::move(high), std::move(step),
                                    std::move(stmts));
}

void Sema::onCaseOfStart(const FilePos &start, const FilePos &, unique_ptr<ExpressionNode> &expr) {
    if (!expr) {
        logger_.error(start, "undefined expression in case statement.");
    }
    const auto eType = expr->getType();
    if (eType->isPointer() || eType->isRecord()) {
        if (auto qExpr = dynamic_cast<QualifiedExpression *>(expr.get())) {
            if (!qExpr->selectors().empty()) {
                // O07.9.5: The type T of the case expression (case variable) may also be a record or pointer type.
                logger_.error(expr->pos(), "non-integer case expression must be a variable or variable parameter.");
            }
            auto decl = qExpr->dereference();
            // If the case variable is a record it must be a variable parameter
            if (eType->isRecord() && (decl->getNodeType() != NodeType::parameter ||
                                      !dynamic_cast<ParameterNode *>(decl)->isVar())) {
                logger_.error(expr->pos(), "record must be a variable parameter.");
            }
            // Save the formal type of the case variable
            caseTys_[qExpr] = eType;
        } else {
            logger_.error(start, "non-integer case expression must be a variable or variable parameter.");
        }
    }
}

unique_ptr<CaseOfNode>
Sema::onCaseOfEnd(const FilePos &start, const FilePos &,
               unique_ptr<ExpressionNode> expr,
               vector<unique_ptr<CaseNode>> cases,
               unique_ptr<StatementSequenceNode> elseStmts) {
    auto eType = expr->getType();
    if (eType->isInteger() || eType->isChar()) {
        // For integer-based case statements, check whether the labels of the cases overlap.
        unordered_set<int64_t> labels;
        for (auto &c: cases) {
            auto label = c->getLabel();
            for (int64_t l: label->getValues()) {
                if (labels.contains(l)) {
                    logger_.error(c->pos(), "duplicate case labels in case statement.");
                    break;
                } else {
                    labels.insert(l);
                }
            }
        }
    } else if (eType->isPointer() || eType->isRecord()) {
        vector<TypeNode *> labels;
        // Check if the types specified by the cases are unique or whether a case subsumes a later case
        for (auto &c: cases) {
            auto label = c->getLabel()->getValue(0);
            if (auto lExpr = dynamic_cast<QualifiedExpression *>(label)) {
                auto decl = dynamic_cast<TypeDeclarationNode *>(lExpr->dereference());
                auto lType = decl->getType();
                for (auto l: labels) {
                    if (l == lType) {
                        logger_.error(label->pos(), "duplicate case labels in case statement.");
                        break;
                    } else if (lType->extends(l)) {
                        logger_.warning(label->pos(), "unreachable case label in case statement.");
                    }
                }
                labels.push_back(lType);
            } else {
                logger_.error(label->pos(), "type mismatch: case label type must be pointer or record.");
            }
        }
        // Clean up at the end of the case statement by deleting the saved formal type of the case expression
        if (auto qExpr = dynamic_cast<QualifiedExpression *>(expr.get())) {
            if (caseTys_[qExpr]) {
                caseTys_.erase(qExpr);
            }
        }
    } else {
        logger_.error(expr->pos(), "type mismatch: case expression type must be integer, character, pointer, or record.");
    }
    return make_unique<CaseOfNode>(start, std::move(expr), std::move(cases), std::move(elseStmts));
}

unique_ptr<CaseLabelNode>
Sema::onCaseLabel(const FilePos &start, const FilePos &,
                  unique_ptr<ExpressionNode> &expr,
                  vector<unique_ptr<ExpressionNode>> labels) {
    set<int64_t> cases;
    TypeNode *common = nullptr;
    for (const auto &label : labels) {
        const auto lType = label->getType();
        if (lType->isInteger() || lType->isChar()) {
            if (common) {
                if ((common->isInteger() && !lType->isInteger()) || (common->isChar() && !lType->isChar())) {
                    logger_.error(label->pos(), "type mismatch: case labels must all have the same type.");
                    continue;
                }
            } else {
                common = lType;
            }
            if (label->getNodeType() == NodeType::range_expression) {
                const auto lExpr = dynamic_cast<RangeExpressionNode *>(label.get());
                auto lower = lExpr->getLower();
                optional<int64_t> loValue;
                if (lower->getType()->isInteger()) {
                    loValue = integer_cast(lower);
                } else if (label->getType()->isChar()) {
                    loValue = char_cast(lower);
                }
                auto upper = lExpr->getUpper();
                optional<int64_t> upValue;
                if (upper->getType()->isInteger()) {
                    upValue = integer_cast(upper);
                } else if (label->getType()->isChar()) {
                    upValue = char_cast(upper);
                }
                if (loValue.has_value() && upValue.has_value()) {
                    if (upValue.value() > loValue.value()) {
                        for (int64_t value = loValue.value(); value <= upValue.value(); ++value) {
                            if (cases.contains(value)) {
                                logger_.error(lExpr->pos(), "duplicate case labels in case statement.");
                            }
                            cases.insert(value);
                        }
                    } else {
                        logger_.error(upper->pos(), "upper bound must be greater than lower bound.");
                    }
                }
            } else if (label->isLiteral()) {
                optional<int64_t> value;
                if (label->getType()->isInteger()) {
                    value = integer_cast(label.get());
                } else if (label->getType()->isChar()) {
                    value = char_cast(label.get());
                } else {
                    logger_.error(label->pos(), "integer or character value expected.");
                }
                if (value.has_value()) {
                    if (cases.contains(value.value())) {
                        logger_.error(label->pos(), "duplicate case labels in case statement.");
                    }
                    cases.insert(value.value());
                }
            }
        } else if (lType->kind() == TypeKind::TYPE) {
            if (labels.size() > 1) {
                logger_.error(labels[1]->pos(), "non-integer case must have a single type as label.");
                break;
            }
            if (auto lExpr = dynamic_cast<QualifiedExpression *>(label.get())) {
                const auto decl = dynamic_cast<TypeDeclarationNode *>(lExpr->dereference());
                const auto pType = decl->getType();
                const auto eType = expr->getType();
                if ((eType->isPointer() && pType->isPointer()) || (eType->isRecord() && pType->isRecord())) {
                    if (!pType->extends(eType)) {
                        logger_.error(label->pos(), "type mismatch: " + format(pType) +
                                                    " is not an extension of " + format(eType) + ".");
                    } else if (auto qExpr = dynamic_cast<QualifiedExpression *>(expr.get())) {
                        // Change the declaration of the case expression to reflect its dynamic type
                        qExpr->dereference()->setType(pType);
                    }
                } else {
                    logger_.error(label->pos(), "type mismatch: case label type " + format(pType) +
                                                " is incompatible with case expression type "+ format(eType) + ".");
                }
            }
        } else {
            logger_.error(label->pos(), "constant expression, record type, or pointer type expected.");
        }
    }
    return make_unique<CaseLabelNode>(start, std::move(labels), std::move(cases));
}

unique_ptr<CaseNode>
Sema::onCase(const FilePos &start, const FilePos &,
             unique_ptr<ExpressionNode> &expr, unique_ptr<CaseLabelNode> label, unique_ptr<StatementSequenceNode> stmts) {
    const auto lType = label->getType();
    if (lType->isInteger() || lType->isChar()) {
        const auto eType = expr->getType();
        if ((eType->isInteger() && !lType->isInteger()) || (eType->isChar() && !lType->isChar())) {
            logger_.error(label->pos(), "type mismatch: case label type is different from case expression type.");
        }
    } else if (lType->kind() == TypeKind::TYPE) {
        // Restore the type of the case expression to its previously saved formal type
        if (auto qExpr = dynamic_cast<QualifiedExpression *>(expr.get())) {
            auto eType = caseTys_[qExpr];
            qExpr->dereference()->setType(eType);
        }
    }
    return make_unique<CaseNode>(start, std::move(label), std::move(stmts));
}

unique_ptr<ReturnNode>
Sema::onReturn(const FilePos &start, const FilePos &, unique_ptr<ExpressionNode> expr) {
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

unique_ptr<ExitNode> Sema::onExit(const FilePos &start, const FilePos &) const {
    if (loops_.empty()) {
        logger_.error(start, "EXIT statement outside of loop.");
    }
    return make_unique<ExitNode>(start);
}

unique_ptr<StatementNode>
Sema::onQualifiedStatement(const FilePos &start, const FilePos &,
                           unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors) {
    DeclarationNode* sym = symbols_->lookup(ident.get());
    if (!sym) {
        logger_.error(ident->start(), "undefined identifier: " + to_string(*ident) + ".");
        return nullptr;
    }
    // Check whether this is an external or imported procedure
    if (const auto proc = dynamic_cast<ProcedureNode *>(sym)) {
        if (ident->isQualified() && proc->isExternal()) {
            // A fully qualified external reference needs to be added to module for code generation
            context_->addExternalProcedure(dynamic_cast<ProcedureDeclarationNode *>(proc));
        }
    }
    const auto type = onSelectors(ident->start(), ident->end(), sym, sym->getType(), selectors);
    if (const auto procTy = dynamic_cast<ProcedureTypeNode *>(type)) {
        // Looks like a procedure reference that needs to be treated as a procedure call
        if (procTy->getReturnType()) {
            logger_.error(ident->start(), "function procedure call must be followed by parameter list.");
        }
        // For uniformity, add an empty parameter list to the procedure call if none is present
        selectors.insert(selectors.end(), make_unique<ActualParameters>(ident->end()));
        return make_unique<QualifiedStatement>(start, std::move(ident), std::move(selectors), sym);
    }
    if (!selectors.empty() && selectors.back()->getNodeType() == NodeType::parameter) {
        // Looks like a proper or function procedure call
        if (type != noTy_) {
            logger_.warning(ident->start(), "discarded expression value.");
        }
        return make_unique<QualifiedStatement>(start, std::move(ident), std::move(selectors), sym);
    }
    logger_.error(ident->start(), "procedure call expected.");
    return nullptr;
}

unique_ptr<QualifiedExpression>
Sema::onQualifiedExpression(const FilePos &start, const FilePos &,
                            unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors) {
    DeclarationNode* sym = symbols_->lookup(ident.get());
    if (!sym) {
        logger_.error(ident->start(), "undefined identifier: " + to_string(*ident) + ".");
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, noTy_);
    }
    // Check whether the qualified identifier is a variable or parameter
    if (sym->getNodeType() == NodeType::variable || sym->getNodeType() == NodeType::parameter) {
        auto type = onSelectors(ident->start(), ident->end(), sym, sym->getType(), selectors);
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, type);
    }
    // Check whether the qualified identifier is a type identifier
    if (sym->getNodeType() == NodeType::type) {
        if (!selectors.empty()) {
            logger_.error(selectors[0]->pos(), "unexpected selector.");
        }
        return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, typeTy_);
    }
    // Check whether the qualified identifier is an external or imported procedure
    if (const auto proc = dynamic_cast<ProcedureNode *>(sym)) {
        if (ident->isQualified() && proc->isExternal()) {
            // A fully qualified external reference needs to be added to module for code generation
            context_->addExternalProcedure(dynamic_cast<ProcedureDeclarationNode *>(proc));
        }
        auto type = onSelectors(ident->start(), ident->end(), sym, sym->getType(), selectors);
        if (type->isProcedure()) {
            // Looks like a reference to a procedure (no return type) or to a function procedure (no selectors)
            if (proc->isPredefined()) {
                logger_.error(start, "predefined procedures cannot be referenced.");
            }
            return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, type);
        }
        if (!selectors.empty() && selectors.back()->getNodeType() == NodeType::parameter) {
            // Looks like a call to a function procedure
            return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, type);
        }
    }
    logger_.error(ident->start(), "variable, parameter, type, or function call expected.");
    return make_unique<QualifiedExpression>(start, std::move(ident), std::move(selectors), sym, noTy_);
}

unique_ptr<ExpressionNode>
Sema::onQualifiedConstant(const FilePos &start, const FilePos &end,
                          const unique_ptr<QualIdent> &ident, const vector<unique_ptr<Selector>> &selectors) {
    DeclarationNode* sym = symbols_->lookup(ident.get());
    if (!sym) {
        logger_.error(ident->start(), "undefined identifier: " + to_string(*ident) + ".");
        return nullptr;
    }
    // Check constant reference
    if (sym->getNodeType() == NodeType::constant) {
        if (!selectors.empty()) {
            const auto sel = selectors[0].get();
            logger_.warning(sel->pos(), "ignoring unexpected selector(s).");
        }
        const auto decl = dynamic_cast<ConstantDeclarationNode *>(sym);
        if (auto opt = fold(start, end, decl->getValue())) {
            return std::move(opt.value());
        }
    }
    logger_.error(ident->start(), "constant expected.");
    return nullptr;
}

TypeNode *
Sema::onSelectors(const FilePos &start, const FilePos &end,
                  DeclarationNode *sym, TypeNode *base, vector<unique_ptr<Selector>> &selectors) {
    if (selectors.size() > 1) {
        handleRepeatedIndices(start, end, selectors);
    }
    auto it = selectors.begin();
    // it = handleMissingParameters(start, end, base, selectors, it);
    auto context = sym;
    while (it != selectors.end()) {
        auto sel = it->get();
        if (!base) {
            logger_.error(sel->pos(), "unexpected selector.");
            return nullptr;
        }
        // Check for implicit pointer de-referencing
        if (base->isPointer() && (sel->getNodeType() == NodeType::array_type ||
                                  sel->getNodeType() == NodeType::record_type)) {
            auto caret = make_unique<Dereference>(sel->pos());
            // Place caret before the current element
            it = selectors.insert(it, std::move(caret));
            sel = it->get();
        }
        // If necessary, convert from actual parameters to type guard
        if (sel->getNodeType() == NodeType::parameter && !base->isProcedure()) {
            const auto params = dynamic_cast<ActualParameters *>(sel);
            if (params->parameters().size() == 1) {
                const auto param = params->parameters()[0].get();
                if (param->getNodeType() == NodeType::qualified_expression &&
                    param->getType()->kind() == TypeKind::TYPE) {
                    const auto expr = dynamic_cast<QualifiedExpression *>(param);
                    *it = make_unique<Typeguard>(params->pos(), make_unique<QualIdent>(expr->ident()));
                    sel = it->get();
                } else {
                    logger_.error(params->pos(), "unexpected selector: illegal type guard.");
                    return nullptr;
                }
            } else {
                logger_.error(params->pos(), "unexpected selector: illegal type guard.");
                return nullptr;
            }
        }
        switch (sel->getNodeType()) {
            case NodeType::parameter:
                base = onActualParameters(context, base, dynamic_cast<ActualParameters *>(sel));
                break;
            case NodeType::array_type:
                base = onArrayIndex(base, dynamic_cast<ArrayIndex *>(sel));
                break;
            case NodeType::pointer_type:
                base = onDereference(base, dynamic_cast<Dereference *>(sel));
                break;
            case NodeType::record_type:
                context = onRecordField(base, dynamic_cast<RecordField *>(sel));
                base = context ? context->getType() : noTy_;
                break;
            case NodeType::type:
                base = onTypeguard(context, base, dynamic_cast<Typeguard *>(sel));
                break;
            default:
                logger_.error(sel->pos(), "unexpected selector.");
                base = noTy_;
        }
        if (!base || base == noTy_) {
            return noTy_;
        }
        // it = handleMissingParameters(start, end, base, selectors, it);
        ++it;
    }
    return base;
}

bool
Sema::assertAssignable(const ExpressionNode *expr, string &err) const {
    if (expr->isLiteral()) {
        err = "a constant value";
        return false;
    }
    if (expr->getNodeType() == NodeType::qualified_expression) {
        if (const auto decl = dynamic_cast<const QualifiedExpression *>(expr)->dereference()) {
            if (decl->getNodeType() == NodeType::parameter) {
                const auto type = decl->getType();
                if (type->isStructured()) {
                    // O07.9.1: If a value parameter is structured (of array or record type),
                    // no assignment to it or to its elements are permitted.
                    const auto param = dynamic_cast<const ParameterNode *>(decl);
                    err = "a non-variable structured parameter";
                    return param->isVar();
                }
            } else if (decl->getNodeType() == NodeType::constant) {
                err = "a constant";
                return false;
            } else if (decl->getNodeType() == NodeType::variable) {
                if (decl->getModule() != context_->getTranslationUnit()) {
                    // O07.11: Variables are always exported in read-only mode.
                    err = "an external variable";
                    return false;
                }
            }
        }
        err = "";
        return true;
    }
    err = "an expression";
    return false;
}

Sema::SelectorIterator &
Sema::handleMissingParameters(const FilePos &start, const FilePos &end,
                              TypeNode *base, Selectors &selectors, SelectorIterator &it) const {
    if (base && base->isProcedure()) {
        bool found = false;
        if (selectors.empty()) {
            it = selectors.insert(selectors.begin(), make_unique<ActualParameters>(end));
            found = true;
        } else if (it + 1 != selectors.end() && (*(it +1))->getNodeType() != NodeType::parameter) {
            it = selectors.insert(it, make_unique<ActualParameters>(end));
            found = true;
        }
        const auto proc = dynamic_cast<ProcedureTypeNode *>(base);
        if (found && proc->getReturnType()) {
            logger_.error(start, "function procedure call must be followed by parameter list.");
        }
    }
    return it;
}

void Sema::handleRepeatedIndices(const FilePos &, const FilePos &, Selectors &selectors) const {
    auto it = selectors.begin();
    while (it != selectors.end()) {
        const auto cur = it->get();
        if (cur->getNodeType() == NodeType::array_type) {
            auto first = it;
            auto last = first;
            while (last != selectors.end() && (*last)->getNodeType() == NodeType::array_type) {
                ++last;
            }
            if (first != last - 1) {
                vector<unique_ptr<ExpressionNode>> indices;
                for (auto acc = first; acc != last; ++acc) {
                    const auto index = dynamic_cast<ArrayIndex *>(acc->get());
                    for (auto &sel: index->indices()) {
                        indices.push_back(std::move(sel));
                    }
                }
                // Report a warning
                logger_.warning((*first)->pos(), "use multi-dimensional index to access multi-dimensional array.");
                // Erase the repeated array indices
                auto pos = selectors.erase(first, last);
                // Place the new combined array index before the current position
                it = selectors.insert(pos, make_unique<ArrayIndex>(EMPTY_POS, std::move(indices)));
            }
        }
        ++it;
    }
}

TypeNode *
Sema::onActualParameters(DeclarationNode *context, TypeNode *base, ActualParameters *sel) {
    if (!base->isProcedure()) {
        logger_.error(sel->pos(), "type " + to_string(base) + " is not a procedure type.");
        return noTy_;
    }
    const auto proc = dynamic_cast<ProcedureTypeNode *>(base);
    if (sel->parameters().size() < proc->parameters().size()) {
        logger_.error(sel->pos(), "fewer actual than formal parameters.");
    }
    vector<TypeNode *> types;
    TypeNode *typeType = noTy_;
    for (size_t cnt = 0; cnt < sel->parameters().size(); cnt++) {
        const auto expr = sel->parameters()[cnt].get();
        if (!expr) {
            continue;
        }
        const auto exprTy = expr->getCast() ? expr->getCast() : expr->getType();
        if (cnt < proc->parameters().size()) {
            const auto param = proc->parameters()[cnt].get();
            const auto paramTy = param->getType();
            if (assertCompatible(expr->pos(), paramTy, exprTy)) {
                if (param->isVar()) {
                    string err;
                    if (!assertAssignable(expr, err)) {
                        logger_.error(expr->pos(), "illegal actual parameter: cannot pass " + err + " by reference.");
                    } else if (exprTy->isNumeric() && paramTy->isNumeric()) {
                        // The types of variable parameters need to be an exact match as they are read-write parameters
                        if (!paramTy->isVirtual() && paramTy->kind() != exprTy->kind()) {
                            logger_.error(expr->pos(), "type mismatch: cannot pass " + to_string(*exprTy->getIdentifier()) +
                                               " to " + to_string(*paramTy->getIdentifier()) + " by reference.");
                        }
                    }
                } else {
                    if (param->getType() != expr->getType()) {
                        if (expr->isLiteral()) {
                            castLiteral(sel->parameters()[cnt], param->getType());
                        } else {
                            // Type inference: compatibility does not check if the `CHAR` value is a literal
                            if (param->getType()->isArray() && expr->getType()->isChar()) {
                                logger_.error(expr->pos(), "type mismatch: cannot pass a non-constant character value to a string parameter.");
                            }
                            cast(expr, param->getType());
                        }
                    }
                }
                types.push_back(sel->parameters()[cnt]->getType());
                if (param->getType()->kind() == TypeKind::TYPE)  {
                    if (expr->getNodeType() == NodeType::qualified_expression) {
                        const auto decl = dynamic_cast<QualifiedExpression *>(expr)->dereference();
                        typeType = decl->getType();
                    }
                }
            } else {
                types.push_back(noTy_);
            }
        } else if (!proc->hasVarArgs()) {
            logger_.error(sel->pos(), "more actual than formal parameters.");
            break;
        }
    }
    // Pseudo-overloading for predefined procedures
    if (context->getNodeType() == NodeType::procedure) {
        const auto decl = dynamic_cast<ProcedureNode *>(context);
        if (decl->isPredefined()) {
            const auto predefined = dynamic_cast<PredefinedProcedure *>(decl);
            if (predefined->isOverloaded()) {
                const auto signature = predefined->dispatch(types, typeType);
                if (signature) {
                    for (size_t cnt = 0; cnt < sel->parameters().size(); cnt++) {
                        const auto param = sel->parameters()[cnt].get();
                        if (param) {
                            param->setCast(nullptr);
                            cast(param, signature->parameters()[cnt]->getType());
                        }
                    }
                    return signature->getReturnType();
                }
            }
        }
    }
    return proc->getReturnType();
}

TypeNode *Sema::onArrayIndex(TypeNode *base, ArrayIndex *sel) const {
    if (!base->isArray()) {
        logger_.error(sel->pos(), format(base) + " is not an array.");
        return noTy_;
    }
    const auto array = dynamic_cast<ArrayTypeNode *>(base);
    if (sel->indices().size() > array->lengths().size()) {
        logger_.error(sel->pos(), "more indices than array dimensions: " + to_string(sel->indices().size())
                                  + " > " + to_string(array->lengths().size()) + ".");
    }
    const auto num = std::min(array->lengths().size(), sel->indices().size());
    for (size_t i = 0; i < num; ++i) {
        const auto index = sel->indices()[i].get();
        const auto type = index->getType();
        if (type->isInteger()) {
            if (index->isLiteral()) {
                const auto literal = dynamic_cast<const IntegerLiteralNode *>(index);
                if (array->isOpen()) {
                    if (literal->value() < 0) {
                        logger_.error(literal->pos(), "negative value " + to_string(literal->value())
                                                      + " is not a valid array index.");
                    }
                } else {
                    const auto length = static_cast<int64_t>(array->lengths()[i]);
                    assertInBounds(literal, 0, length - 1);
                }
            }
        } else {
            logger_.error(sel->pos(), "integer expression expected.");
        }
    }
    return array->types()[num - 1];
}

TypeNode *Sema::onDereference(TypeNode *base, const Dereference *sel) const {
    if (!base->isPointer()) {
        logger_.error(sel->pos(), "pointer " + to_string(base) + " is not a pointer pointer.");
        return noTy_;
    }
    const auto pointer = dynamic_cast<PointerTypeNode *>(base);
    return pointer->getBase();
}

FieldNode *Sema::onRecordField(TypeNode *base, RecordField *sel) const {
    if (!base->isRecord()) {
        logger_.error(sel->pos(), "record " + to_string(base) + " is not a record record.");
        return nullptr;
    }
    const auto record = dynamic_cast<RecordTypeNode *>(base);
    const auto field = record->getField(sel->ident()->name());
    if (!field) {
        logger_.error(sel->pos(), "undefined record field for type " + to_string(base) + ": " + to_string(*sel->ident()) + ".");
        return nullptr;
    }
    sel->setField(field);
    return field;
}

TypeNode *Sema::onTypeguard(DeclarationNode *sym, TypeNode *base, Typeguard *sel) const {
    const FilePos start = sel->ident()->start();
    auto decl = symbols_->lookup(sel->ident());
    if (decl) {
        // O07.8.1: in v(T), v is a variable parameter of record type, or v is a pointer.
        const auto actual = base->isPointer() ? base : sym->getType();
        if (actual->isPointer() || (actual->isRecord() &&
                                    sym->getNodeType() == NodeType::parameter &&
                                    dynamic_cast<ParameterNode *>(sym)->isVar())) {
            if (decl->getNodeType() == NodeType::type) {
                const auto guard = dynamic_cast<TypeDeclarationNode *>(decl)->getType();
                if (guard->isPointer() || guard->isRecord()) {
                    if (!guard->extends(actual)) {
                        logger_.error(start, "type mismatch: " + format(guard) + " is not an extension of "
                                             + format(actual) + ".");
                    } else if (actual->extends(guard)) {
                        logger_.warning(start, "type check is always true.");
                    }
                } else {
                    logger_.error(start, "type mismatch: record type or pointer to record type expected.");
                }
                sel->setType(guard);
                return guard;
            }
            logger_.error(start, "unexpected selector.");
        } else {
            logger_.error(start, "type mismatch: a type guard can only be applied to a variable parameter of record type or a pointer.");
        }
    } else {
        logger_.error(start, "undefined identifier: " + to_string(*sel->ident()) + ".");
    }
    return noTy_;
}

unique_ptr<ExpressionNode>
Sema::onUnaryExpression(const FilePos &start, const FilePos &end,
                        OperatorType op, unique_ptr<ExpressionNode> expr) {
    if (!expr) {
        logger_.error(start, "undefined expression in unary expression.");
        return nullptr;
    }
    auto type = expr->getType();
    if (!type) {
        logger_.error(start, "undefined type in unary expression.");
        type = noTy_;
    }
    if (auto opt = fold(start, end, op, expr)) {
        return std::move(opt.value());
    }
    return make_unique<UnaryExpressionNode>(start, op, std::move(expr), type);
}

unique_ptr<ExpressionNode>
Sema::onBinaryExpression(const FilePos &start, const FilePos &end,
                         OperatorType op, unique_ptr<ExpressionNode> lhs, unique_ptr<ExpressionNode> rhs) {
    if (!lhs) {
        logger_.error(start, "undefined left-hand side in binary expression.");
        return nullptr;
    }
    if (!rhs) {
        logger_.error(start, "undefined right-hand side in binary expression.");
        return nullptr;
    }
    const auto lhsType = lhs->getType();
    if (!lhsType) {
        logger_.error(lhs->pos(), "undefined left-hand side type in binary expression.");
        return nullptr;
    }
    const auto rhsType = rhs->getType();
    if (!rhsType) {
        logger_.error(rhs->pos(), "undefined right-hand side type in binary expression.");
        return nullptr;
    }
    TypeNode *common = nullptr;
    TypeNode *result = nullptr;
    switch (op) {
        case OperatorType::EQ:
        case OperatorType::NEQ:
            if ((lhs->isLiteral() && (lhsType->isString() || lhsType->isChar())) ||
                (rhs->isLiteral() && (rhsType->isString() || rhsType->isChar()))) {
                if ((lhs->isLiteral() && rhsType->isArray()) || (lhsType->isArray() && rhs->isLiteral())) {
                    const auto type = lhsType->isArray() ? lhsType : rhsType;
                    const auto &literal = lhs->isLiteral() ? lhs : rhs;
                    if (const auto array = dynamic_cast<ArrayTypeNode *>(type)) {
                        if (array->dimensions() == 1 && array->getMemberType()->isChar()) {
                            if (!array->isOpen() && literal->getType()->isString()) {
                                const auto str = dynamic_cast<StringLiteralNode *>(literal.get());
                                if (str->value().size() + 1 > array->lengths()[0]) {
                                    logger_.warning(literal->pos(), "string literal is longer than length of character array.");
                                }
                            }
                            result = boolTy_;
                            break;
                        }
                    }
                } else if (lhs->isLiteral() && rhs->isLiteral()) {
                    common = commonType(start, lhsType, rhsType);
                    result = boolTy_;
                    break;
                }
            } else if (lhsType->isArray() && rhsType->isArray()) {
                const auto lhsArray = dynamic_cast<ArrayTypeNode *>(lhsType);
                const auto rhsArray = dynamic_cast<ArrayTypeNode *>(rhsType);
                if (lhsArray->dimensions() == 1 && rhsArray->dimensions() == 1 &&
                    lhsArray->getMemberType()->isChar() && rhsArray->getMemberType()->isChar()) {
                    result = boolTy_;
                } else {
                    logger_.error(lhs->pos(), "comparison operator requires one-dimensional character array arguments.");
                }
                break;
            } else if ((lhsType->isPointer() && rhsType->isPointer()) ||
                       (lhsType->isPointer() && rhsType->kind() == TypeKind::NILTYPE) ||
                       (lhsType->kind() == TypeKind::NILTYPE && rhsType->isPointer()) ||
                       (lhsType->kind() == TypeKind::NILTYPE && rhsType->kind() == TypeKind::NILTYPE)) {
                common = commonType(start, lhsType, rhsType);
                result = boolTy_;
                break;
            } else if (lhsType->isBoolean() && rhsType->isBoolean()) {
                result = boolTy_;
                break;
            }
            [[fallthrough]];
        case OperatorType::LEQ:
        case OperatorType::GEQ:
            if ((lhsType->isNumeric() && rhsType->isNumeric()) ||
                (lhsType->isChar() && rhsType->isChar()) ||
                (lhsType->isSet() && rhsType->isSet())) {
                common = commonType(start, lhsType, rhsType);
                result = boolTy_;
            } else {
                logger_.error(lhs->pos(), "comparison operation requires numeric arguments.");
            }
            break;
        case OperatorType::LT:
        case OperatorType::GT:
            if ((lhsType->isNumeric() && rhsType->isNumeric()) ||
                (lhsType->isChar() && rhsType->isChar())) {
                common = commonType(start, lhsType, rhsType);
                result = boolTy_;
            } else {
                logger_.error(lhs->pos(), "comparison operation requires numeric arguments.");
            }
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
            if (lhsType->isString() || rhsType->isString()) {
                if (lhs->isLiteral() && rhs->isLiteral()) {
                    common = commonType(start, lhsType, rhsType);
                    result = common;
                } else {
                    logger_.error(lhs->pos(), "string concatenation requires constant arguments.");
                }
                break;
            }
            [[fallthrough]];
        case OperatorType::MINUS:
        case OperatorType::TIMES:
            if ((lhsType->isNumeric() && rhsType->isNumeric())
                || (lhsType->isSet() && rhsType->isSet())){
                common = commonType(start, lhsType, rhsType);
                result = common;
            } else {
                logger_.error(lhs->pos(), "arithmetic operation requires numeric or set arguments.");
            }
            break;
        case OperatorType::DIV:
        case OperatorType::MOD:
            if ((lhsType->isInteger() || lhsType->isByte()) &&
                (rhsType->isInteger() || rhsType->isByte())) {
                common = commonType(start, lhsType, rhsType);
                result = common;
            } else {
                logger_.error(start, "integer division requires integer arguments.");
            }
            break;
        case OperatorType::DIVIDE:
            if ((lhsType->isNumeric() && rhsType->isNumeric())
                || (lhsType->isSet() && rhsType->isSet())) {
                common = commonType(start, lhsType, rhsType);
                result = common;
                if (common->isInteger()) {
                    common = this->realTy_;
                    result = common;
                }
            } else {
                logger_.error(lhs->pos(), "arithmetic operation requires numeric or set arguments.");
            }
            break;
        case OperatorType::OR:
        case OperatorType::AND:
            if (lhsType->isBoolean() && rhsType->isBoolean()) {
                common = boolTy_;
                result = common;
            } else {
                logger_.error(start, "operator " + to_string(op) + " requires boolean arguments.");
            }
            break;
        case OperatorType::IS:
            if (lhs->getNodeType() != NodeType::qualified_expression ||
                rhs->getNodeType() != NodeType::qualified_expression) {
                logger_.error(start, "type test operator requires qualified expressions as arguments.");
                break;
            }
            if (rhsType->kind() == TypeKind::TYPE) {
                const auto lhsDecl = dynamic_cast<QualifiedExpression *>(lhs.get())->dereference();
                const auto rhsDecl = dynamic_cast<QualifiedExpression *>(rhs.get())->dereference();
                const auto type = rhsDecl->getType();
                if (lhsType->isPointer() || (lhsType->isRecord() &&
                                             lhsDecl->getNodeType() == NodeType::parameter &&
                                             dynamic_cast<ParameterNode *>(lhsDecl)->isVar())) {
                    if (type->isPointer() || type->isRecord()) {
                        if (lhsType->extends(type)) {
                            logger_.warning(start, "type check is always true.");
                        } else if (!type->extends(lhsType)) {
                            logger_.error(rhs->pos(), "type mismatch: " + format(type) + " is not an extension of " +
                                                      format(lhsType) + ".");
                        }
                    } else {
                        logger_.error(rhs->pos(), "type mismatch: record type or pointer to record type expected.");
                    }
                } else {
                    logger_.error(start, "variable parameter of record type or expression of pointer type expected.");
                }
            } else {
                logger_.error(rhs->pos(), "type identifier expected.");
            }
            result = boolTy_;
            break;
        default:
            logger_.error(start, "unsupported operator: " + to_string(op) + ".");
    }
    if (!result) {
        logger_.error(start, "could not infer result type of expression.");
        result = noTy_;
    }
    // Folding
    if (result != noTy_) {
        if (auto opt = fold(start, end, op, lhs, rhs, result)) {
            if (opt.value()->getType()->isBoolean()) {
                const bool value = dynamic_cast<BooleanLiteralNode *>(opt.value().get())->value();
                const string msg = value ? "true." : "false.";
                logger_.warning(start, "condition is always " + msg);
            }
            return std::move(opt.value());
        }
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
Sema::onRangeExpression(const FilePos &start, const FilePos &,
                        unique_ptr<ExpressionNode> lower, unique_ptr<ExpressionNode> upper) const {
    if (!lower) {
        logger_.error(start, "undefined lower bound in range expression.");
        return nullptr;
    }
    const auto loType = lower->getType();
    if (!loType->isInteger() && !loType->isChar()) {
        logger_.error(lower->pos(), "range expression requires integer or character values.");
    }
    if (!upper) {
        logger_.error(start, "undefined upper bound in range expression.");
        return nullptr;
    }
    const auto upType = upper->getType();
    if (!upType->isInteger() && !upType->isChar()) {
        logger_.error(upper->pos(), "range expression requires integer or character values.");
    }
    if ((loType->isInteger() && upType->isChar()) || (loType->isChar() && upType->isInteger())) {
        logger_.error(start, "type of lower and upper bound in range expression do not match.");
    }
    auto common = loType;
    if (loType->getSize() > upType->getSize()) {
        cast(upper.get(), loType);
    } else if (loType->getSize() < upType->getSize()) {
        cast(lower.get(), upType);
        common = upType;
    }
    return make_unique<RangeExpressionNode>(start, std::move(lower), std::move(upper), common);
}

unique_ptr<ExpressionNode>
Sema::onSetExpression(const FilePos &start, const FilePos &,
                      vector<unique_ptr<ExpressionNode>> elements) {
    int64_t last = -1;
    for (auto &elem : elements) {
        if (!elem->getType()->isInteger()) {
            logger_.error(elem->pos(), "integer expression expected.");
        } else if (elem->getNodeType() == NodeType::range_expression) {
            const auto range = dynamic_cast<RangeExpressionNode *>(elem.get());
            const auto lower = range->getLower();
            int64_t loValue = -1;
            if (lower->isLiteral()) {
                loValue = assertInBounds(dynamic_cast<const IntegerLiteralNode *>(lower), 0, 31);
                if (loValue <= last) {
                    logger_.error(range->getLower()->pos(), "element must be larger than previous element.");
                }
                last = loValue;
            }
            const auto upper = range->getUpper();
            int64_t upValue = -1;
            if (upper->isLiteral()) {
                upValue = assertInBounds(dynamic_cast<const IntegerLiteralNode *>(upper), 0, 31);
                last = upValue;
            }
            if (loValue >= 0 && upValue >= 0) {
                if (loValue >= upValue) {
                    logger_.error(upper->pos(), "upper bound must be greater than lower bound.");
                }
                bitset<32> result;
                result.flip();
                result >>= static_cast<size_t>(loValue);
                result <<= static_cast<size_t>(31 - upValue + loValue);
                result >>= static_cast<size_t>(31 - upValue);
                auto loType = lower->getType();
                auto upType = upper->getType();
                auto common = loType->getSize() > upType->getSize() ? loType : upType;
                elem = make_unique<RangeLiteralNode>(start, result, loValue, upValue, common);
            }
        } else {
            if (elem->isLiteral()) {
                const auto value = assertInBounds(dynamic_cast<const IntegerLiteralNode *>(elem.get()), 0, 31);
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
                const auto range = dynamic_cast<const RangeLiteralNode *>(elem.get());
                result |= range->value();
            } else if (elem->getType()->isInteger()) {
                if (auto pos = integer_cast(elem.get())) {
                    result.set(static_cast<size_t>(pos.value()));
                }
            }
        }
        return make_unique<SetLiteralNode>(start, result, setTy_);
    }
    return expr;
}

int64_t
Sema::assertInBounds(const IntegerLiteralNode *literal, const int64_t lower, const int64_t upper) const {
    const int64_t value = literal->value();
    if (value < lower || value > upper) {
        logger_.error(literal->pos(), "value " + to_string(value) + " out of bounds [" +
                                      to_string(lower) + ".." + to_string(upper) + "].");
        return value < lower ? lower : upper;
    }
    return value;
}

unique_ptr<BooleanLiteralNode>
Sema::onBooleanLiteral(const FilePos &start, const FilePos &, bool value) {
    return make_unique<BooleanLiteralNode>(start, value, boolTy_);
}

unique_ptr<IntegerLiteralNode>
Sema::onIntegerLiteral(const FilePos &start, const FilePos &, int64_t value, const TypeKind kind) const {
    TypeNode *type;
    switch (kind) {
        case TypeKind::SHORTINT: type = shortIntTy_; break;
        case TypeKind::INTEGER: type = integerTy_; break;
        case TypeKind::LONGINT: type = longIntTy_; break;
        default:
            type = noTy_;
    }
    return make_unique<IntegerLiteralNode>(start, value, type);
}

unique_ptr<RealLiteralNode>
Sema::onRealLiteral(const FilePos &start, const FilePos &, double value, const TypeKind kind) const {
    TypeNode *type;
    switch (kind) {
        case TypeKind::REAL: type = realTy_; break;
        case TypeKind::LONGREAL: type = longRealTy_; break;
        default:
            type = noTy_;
    }
    return make_unique<RealLiteralNode>(start, value, type);
}

unique_ptr<StringLiteralNode>
Sema::onStringLiteral(const FilePos &start, const FilePos &, const string &value) {
    return make_unique<StringLiteralNode>(start, value, stringTy_);
}

unique_ptr<CharLiteralNode>
Sema::onCharLiteral(const FilePos &start, const FilePos &, uint8_t value) {
    return make_unique<CharLiteralNode>(start, value, charTy_);
}

unique_ptr<NilLiteralNode>
Sema::onNilLiteral(const FilePos &start, const FilePos &) const {
    return make_unique<NilLiteralNode>(start, symbols_->getNilType());
}

unique_ptr<SetLiteralNode>
Sema::onSetLiteral(const FilePos &start, const FilePos &, bitset<32> value) {
    return make_unique<SetLiteralNode>(start, value, setTy_);
}

bool Sema::isDefined(Ident *ident) const {
    return symbols_->lookup(ident) != nullptr;
}

bool Sema::isConstant(QualIdent *ident) const {
    const auto sym = symbols_->lookup(ident);
    return sym && sym->getNodeType() == NodeType::constant;
}

bool Sema::isType(QualIdent *ident) const {
    const auto sym = symbols_->lookup(ident);
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

int64_t
Sema::euclidean_mod(const int64_t x, const int64_t y) {
    int64_t r = x % y;
    r += y & -(r < 0);
    return r;
}

int64_t
Sema::floor_div(const int64_t x, const int64_t y) {
    const int64_t d = x / y;
    const int64_t r = x % y;
    return r ? d - ((x < 0) ^ (y < 0)) : d;
}

template<typename L, typename T>
optional<unique_ptr<LiteralNode<T>>>
Sema::clone(const FilePos &start, const FilePos &, LiteralNode<T> *literal) {
    if (literal) {
        T value = literal->value();
        return make_unique<L>(start, value, literal->getType(), literal->getCast());
    }
    return std::nullopt;
}

optional<bool>
Sema::boolean_cast(const ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::boolean) {
        return { dynamic_cast<const BooleanLiteralNode *>(expr)->value() };
    }
    return std::nullopt;
}

optional<int64_t>
Sema::integer_cast(const ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::integer) {
        return { dynamic_cast<const IntegerLiteralNode *>(expr)->value() };
    }
    return std::nullopt;
}

optional<uint8_t>
Sema::char_cast(const ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::character) {
        return { dynamic_cast<const CharLiteralNode *>(expr)->value() };
    }
    return std::nullopt;
}

optional<double>
Sema::real_cast(const ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::real) {
        return { dynamic_cast<const RealLiteralNode *>(expr)->value() };
    } else if (expr->getNodeType() == NodeType::integer) {
        // Promote integer literal to real literal
        auto value = dynamic_cast<const IntegerLiteralNode *>(expr)->value();
        return { static_cast<double>(value) };
    }
    return std::nullopt;
}

optional<string>
Sema::string_cast(const ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::string) {
        return dynamic_cast<const StringLiteralNode *>(expr)->value();
    } else if (expr->getNodeType() == NodeType::character) {
        // Promote character literal to string literal
        auto value = dynamic_cast<const CharLiteralNode *>(expr)->value();
        return { string{static_cast<char>(value)} };
    }
    return std::nullopt;
}

optional<bitset<32>>
Sema::set_cast(const ExpressionNode *expr) {
    if (expr->getNodeType() == NodeType::set) {
        return { dynamic_cast<const SetLiteralNode *>(expr)->value() };
    }
    return std::nullopt;
}

optional<unique_ptr<ExpressionNode>>
Sema::fold(const FilePos &start, const FilePos &end, ExpressionNode *expr) {
    if (expr && expr->isLiteral()) {
        auto type = expr->getType();
        if (type) {
            if (type->kind() == TypeKind::CHAR) {
                return clone<CharLiteralNode>(start, end, dynamic_cast<CharLiteralNode*>(expr));
            } else if (type->kind() == TypeKind::SHORTINT ||
                       type->kind() == TypeKind::INTEGER ||
                       type->kind() == TypeKind::LONGINT) {
                return clone<IntegerLiteralNode>(start, end, dynamic_cast<IntegerLiteralNode*>(expr));
            } else if (type->kind() == TypeKind::REAL ||
                       type->kind() == TypeKind::LONGREAL) {
                return clone<RealLiteralNode>(start, end, dynamic_cast<RealLiteralNode*>(expr));
            } else if (type->kind() == TypeKind::BOOLEAN) {
                return clone<BooleanLiteralNode>(start, end, dynamic_cast<BooleanLiteralNode*>(expr));
            } else if (type->kind() == TypeKind::STRING) {
                return clone<StringLiteralNode>(start, end, dynamic_cast<StringLiteralNode*>(expr));
            } else if (type->kind() == TypeKind::SET) {
                return clone<SetLiteralNode>(start, end, dynamic_cast<SetLiteralNode*>(expr));
            }
            logger_.error(start, "unsupported literal type: " + to_string(*type->getIdentifier()) + ".");
        } else {
            logger_.error(start, "unsupported literal type.");
        }
    }
    return std::nullopt;
}

optional<unique_ptr<ExpressionNode>>
Sema::fold(const FilePos &start, const FilePos &, OperatorType op, unique_ptr<ExpressionNode> &expr) {
    auto type = expr->getType();
    auto cast = expr->getCast();
    if (type->isBoolean()) {
        if (auto opt = boolean_cast(expr.get())) {
            switch (op) {
                case OperatorType::NOT:
                    return make_unique<BooleanLiteralNode>(start, !opt.value(), type, cast);
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to boolean values.");
            }
        }
    } else if (type->isInteger()) {
        if (auto opt = integer_cast(expr.get())) {
            switch (op) {
                case OperatorType::PLUS:
                    return make_unique<IntegerLiteralNode>(start, opt.value(), type, cast);
                case OperatorType::NEG: {
                    // Negating an integer literal can change its type from LONGINT to INTEGER or INTEGER to SHORTINT
                    int64_t value = -opt.value();
                    return make_unique<IntegerLiteralNode>(start, value, intType(value), cast);
                }
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
            }
        }
    } else if (type->isReal()) {
        if (auto opt = real_cast(expr.get())) {
            switch (op) {
                case OperatorType::PLUS:
                    return make_unique<RealLiteralNode>(start, opt.value(), type, cast);
                case OperatorType::NEG:
                    return make_unique<RealLiteralNode>(start, -opt.value(), type, cast);
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to real values.");
            }
        }
    } else if (type->isSet()) {
        if (auto opt = set_cast(expr.get())) {
            switch (op) {
                case OperatorType::NEG:
                    return make_unique<SetLiteralNode>(start, opt.value().flip(), type, cast);
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to set values.");
            }
        }
    }
    return std::nullopt;
}

template<typename L, typename T>
optional<unique_ptr<ExpressionNode>>
Sema::foldBinaryOp(const FilePos &start, const FilePos &,
                   unique_ptr<ExpressionNode> &lhs, unique_ptr<ExpressionNode> &rhs,
                   optional<T> neutral, optional<T> zero, function<T(T, T)> op, TypeNode *common) {
    optional<T> lvalue = literal_cast<T>(*lhs);
    optional<T> rvalue = literal_cast<T>(*rhs);
    if (lvalue && rvalue) {
        T result = op(lvalue.value(), rvalue.value());
        return make_unique<L>(start, result, common->isInteger() ? intType(static_cast<int64_t>(result)) : common);
    }
    if (lvalue) {
        if (neutral && neutral.value() == lvalue.value()) {
            return optional(std::move(rhs));
        }
        if (zero && zero.value() == lvalue.value()) {
            return make_unique<L>(start, 0, common->isInteger() ? shortIntTy_ : common);
        }
    }
    if (rvalue) {
        if (neutral && neutral.value() == rvalue.value()) {
            return optional(std::move(lhs));
        }
        if (zero && zero.value() == rvalue.value()) {
            return make_unique<L>(start, 0, common->isInteger() ? shortIntTy_ : common);
        }
    }
    return std::nullopt;
}

optional<unique_ptr<ExpressionNode>>
Sema::foldBooleanOp(const FilePos &start, const FilePos &end,
                    OperatorType op, unique_ptr<ExpressionNode> &lhs, unique_ptr<ExpressionNode> &rhs, TypeNode* common) {
    switch (op) {
        case OperatorType::AND:
            return foldBinaryOp<BooleanLiteralNode, bool>(start, end, lhs, rhs, true, {}, std::logical_and(), common);
        case OperatorType::OR:
            return foldBinaryOp<BooleanLiteralNode, bool>(start, end, lhs, rhs, false, {}, std::logical_or(), common);
        case OperatorType::EQ:
            return foldBinaryOp<BooleanLiteralNode, bool>(start, end, lhs, rhs, {}, {}, std::equal_to(), common);
        case OperatorType::NEQ:
            return foldBinaryOp<BooleanLiteralNode, bool>(start, end, lhs, rhs, {}, {}, std::not_equal_to(), common);
        default:
            logger_.error(start, "operator " + to_string(op) + " cannot be applied to boolean values.");
            return std::nullopt;
    }
}

template<typename T>
optional<unique_ptr<BooleanLiteralNode>>
Sema::foldRelationOp(const FilePos &start, const FilePos &,
                     const OperatorType op, unique_ptr<ExpressionNode> &lhs, unique_ptr<ExpressionNode> &rhs,
                     TypeNode *common) {
    auto lopt = literal_cast<T>(*lhs);
    auto ropt = literal_cast<T>(*rhs);
    if (lopt && ropt) {
        T lvalue = lopt.value();
        T rvalue = ropt.value();
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
                logger_.error(start, "operator " + to_string(op) + " does not return a boolean value.");
        }
    }
    return std::nullopt;
}

optional<unique_ptr<ExpressionNode>>
Sema::foldDivModOp(const FilePos &start, const FilePos &,
                   OperatorType op, unique_ptr<ExpressionNode> &lhs, unique_ptr<ExpressionNode> &rhs, TypeNode *common) {
    auto lopt = integer_cast(lhs.get());
    auto ropt = integer_cast(rhs.get());
    if (ropt) {
        auto rvalue = ropt.value();
        if (rvalue == 0) {
            logger_.error(rhs->pos(), "division by zero.");
            return std::nullopt;
        }
        if (rvalue < 0) {
            logger_.error(rhs->pos(), "divisor cannot be negative.");
            return std::nullopt;
        }
        if (rvalue == 1) {
            if (lopt) {
                return make_unique<IntegerLiteralNode>(start, lopt.value(), common);
            }
            return std::move(lhs);
        }
        if (lopt) {
            auto lvalue = lopt.value();
            auto result = op == OperatorType::DIV ? floor_div(lvalue, rvalue) : euclidean_mod(lvalue, rvalue);
            return make_unique<IntegerLiteralNode>(start, result, common);
        }
    }
    return std::nullopt;
}

optional<unique_ptr<ExpressionNode>>
Sema::foldFDivOp(const FilePos &start, const FilePos &,
                 unique_ptr<ExpressionNode> &lhs, unique_ptr<ExpressionNode> &rhs, TypeNode *common) {
    auto lopt = real_cast(lhs.get());
    auto ropt = real_cast(rhs.get());
    if (ropt) {
        auto rvalue = ropt.value();
        if (rvalue == 0) {
            logger_.error(rhs->pos(), "division by zero.");
            return std::nullopt;
        }
        if (rvalue == 1) {
            if (lopt) {
                return make_unique<RealLiteralNode>(start, lopt.value(), common);
            }
            lhs->setCast(common);
            return std::move(lhs);
        }
        if (lopt) {
            return make_unique<RealLiteralNode>(start, lopt.value() / rvalue, common);
        }
    }
    return std::nullopt;
}

template<typename L, typename T>
optional<unique_ptr<ExpressionNode>>
Sema::foldSubOp(const FilePos &start, const FilePos &,
                unique_ptr<ExpressionNode> &lhs, unique_ptr<ExpressionNode> &rhs, TypeNode *common) {
    auto lopt = literal_cast<T>(*lhs);
    auto ropt = literal_cast<T>(*rhs);
    if (lopt && ropt) {
        T result = lopt.value() - ropt.value();
        return make_unique<L>(start, result, common->isInteger() ? intType(static_cast<int64_t>(result)) : common);
    }
    if (lopt && lopt.value() == 0) {
        return make_unique<UnaryExpressionNode>(start, OperatorType::NEG, std::move(rhs), common);
    }
    if (ropt && ropt.value() == 0) {
        return std::move(lhs);
    }
    return std::nullopt;
}

optional<unique_ptr<ExpressionNode>>
Sema::fold(const FilePos &start, const FilePos &end,
           OperatorType op, unique_ptr<ExpressionNode> &lhs, unique_ptr<ExpressionNode> &rhs, TypeNode* common) {
    if (common->isBoolean()) {
        if (lhs->getType()->isBoolean() && rhs->getType()->isBoolean()) {
            return foldBooleanOp(start, end, op, lhs, rhs, common);
        }
        if (lhs->getType()->isNumeric() && rhs->getType()->isNumeric()) {
            if (lhs->getType()->isInteger() && rhs->getType()->isInteger()) {
                return foldRelationOp<int64_t>(start, end, op, lhs, rhs, common);
            }
            return foldRelationOp<double>(start, end, op, lhs, rhs, common);
        }
        if (lhs->getType()->isChar() && rhs->getType()->isChar()) {
            return foldRelationOp<uint8_t>(start, end, op, lhs, rhs, common);
        }
        if ((lhs->getType()->isString() || lhs->getType()->isChar()) &&
            (rhs->getType()->isString() || rhs->getType()->isChar())) {
            auto lopt = string_cast(lhs.get());
            auto ropt = string_cast(rhs.get());
            if (lopt && ropt) {
                string lvalue = lopt.value();
                string rvalue = ropt.value();
                switch (op) {
                    case OperatorType::EQ: {
                        return make_unique<BooleanLiteralNode>(start, lopt.value() == ropt.value(), boolTy_);
                    }
                    case OperatorType::NEQ: {
                        return make_unique<BooleanLiteralNode>(start, lopt.value() != ropt.value(), boolTy_);
                    }
                    default:
                        logger_.error(start, "operator " + to_string(op) + " does not return a boolean value.");
                }
            }
        } else if (lhs->getType()->kind() == TypeKind::NILTYPE && rhs->getType()->kind() == TypeKind::NILTYPE) {
            switch (op) {
                case OperatorType::EQ:
                    return make_unique<BooleanLiteralNode>(start, true, common);
                case OperatorType::NEQ:
                    return make_unique<BooleanLiteralNode>(start, false, common);
                default:
                    logger_.error(start, "operator " + to_string(op) + " does not return a boolean value.");
            }
        } else if (rhs->getType()->isSet()) {
            auto ropt = set_cast(rhs.get());
            if (ropt) {
                auto rvalue = ropt.value();
                if (lhs->getType()->isSet()) {
                    auto lopt = set_cast(lhs.get());
                    if (lopt) {
                        auto lvalue = lopt.value();
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
                                logger_.error(start, "operator " + to_string(op) + " does not return a boolean value.");
                        }
                    }
                } else if (op == OperatorType::IN && lhs->getType()->isInteger()) {
                    auto lvalue = assertInBounds(dynamic_cast<IntegerLiteralNode *>(lhs.get()), 0, 31);
                    return make_unique<BooleanLiteralNode>(start, rvalue.test(std::size_t(lvalue)), common);
                } else {
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied here.");
                }
            }
        }
    } else if (common->isInteger()) {
        switch (op) {
            case OperatorType::PLUS:
                return foldBinaryOp<IntegerLiteralNode, int64_t>(start, end, lhs, rhs, 0, {}, std::plus(), common);
            case OperatorType::MINUS:
                return foldSubOp<IntegerLiteralNode, int64_t>(start, end, lhs, rhs, common);
            case OperatorType::TIMES:
                return foldBinaryOp<IntegerLiteralNode, int64_t>(start, end, lhs, rhs, 1, 0, std::multiplies(), common);
            case OperatorType::DIV:
            case OperatorType::MOD:
                return foldDivModOp(start, end, op, lhs, rhs, common);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to integer values.");
        }
    } else if (common->isReal()) {
        switch (op) {
            case OperatorType::PLUS:
                return foldBinaryOp<RealLiteralNode, double>(start, end, lhs, rhs, 0, {}, std::plus(), common);
            case OperatorType::MINUS:
                return foldSubOp<RealLiteralNode, double>(start, end, lhs, rhs, common);
            case OperatorType::TIMES:
                return foldBinaryOp<RealLiteralNode, double>(start, end, lhs, rhs, 1, 0, std::multiplies(), common);
            case OperatorType::DIVIDE:
                return foldFDivOp(start, end, lhs, rhs, common);
            default:
                logger_.error(start, "operator " + to_string(op) + " cannot be applied to real values.");
        }
    } else if (common->isString()) {
        auto lopt = string_cast(lhs.get());
        auto ropt = string_cast(rhs.get());
        if (lopt && ropt) {
            switch (op) {
                case OperatorType::PLUS:
                    return make_unique<StringLiteralNode>(start, lopt.value() + ropt.value(), common);
                default:
                    logger_.error(start, "operator " + to_string(op) + " cannot be applied to string values.");
            }
        }
    } else if (common->isSet()) {
        auto lopt = set_cast(lhs.get());
        auto ropt = set_cast(rhs.get());
        if (lopt && ropt) {
            auto lvalue = lopt.value();
            auto rvalue = ropt.value();
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
    }
    return std::nullopt;
}

bool
Sema::assertEqual(Ident *aIdent, Ident *bIdent) {
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
Sema::assertUnique(const IdentDef *ident, DeclarationNode *node) const {
    if (symbols_->isDuplicate(ident->name())) {
        logger_.error(ident->start(), "duplicate definition: " + ident->name() + ".");
    }
    if (symbols_->isGlobal(ident->name())) {
        logger_.error(ident->start(), "predefined identifier: " + ident->name() + ".");
    }
    symbols_->insert(ident->name(), node);
}

void
Sema::checkExport(DeclarationNode *node) const {
    if (node->getIdentifier()->isExported()) {
        if (node->getScope() != SymbolTable::MODULE_SCOPE) {
            logger_.error(node->getIdentifier()->start(), "only top-level declarations can be exported.");
        }
    } else {
        if (node->getNodeType() == NodeType::type) {
            auto decl = dynamic_cast<TypeDeclarationNode *>(node);
            if (decl->getType()->kind() == TypeKind::RECORD) {
                const auto type = dynamic_cast<RecordTypeNode *>(decl->getType());
                for (size_t i = 0; i < type->getFieldCount(); i++) {
                    auto field = type->getField(i);
                    if (field->getIdentifier()->isExported()) {
                        logger_.error(field->pos(), "cannot export fields of non-exported record type.");
                    }
                }
            }
        }
    }
}

bool
Sema::assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual, const bool isPtr) {
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
        // Check virtual numeric compound types
        if ((expected->kind() == TypeKind::ENTIRE && actual->isInteger()) ||
            (expected->kind() == TypeKind::FLOATING && actual->isReal()) ||
            (expected->kind() == TypeKind::NUMERIC)) {
            return true;
        }
        // Both expected and actual type are concrete types: assure legal conversion and promotion
        if ((expected->isInteger() && actual->isInteger()) ||
            (expected->isReal() && actual->isReal())) {
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
        if ((expected->isByte() && actual->isInteger()) ||
            (expected->isInteger() && actual->isByte())) {
            return true;
        }
    }
    // Check array type
    if (expected->isArray()) {
        auto exp_array = dynamic_cast<ArrayTypeNode *>(expected);
        if (actual->isArray()) {
            if (exp_array->getMemberType()->kind() == TypeKind::ANYTYPE) {
                // If the expected member type is `ANYTYPE`, as for example with the LEN procedure,
                // the dimensions cannot be checked as `ANYTYPE` can also be a nested ARRAY type.
                return true;
            }
            auto act_array = dynamic_cast<ArrayTypeNode *>(actual);
            if (exp_array->dimensions() >= act_array->dimensions()) {
                if (exp_array->isOpen()) {
                    return assertCompatible(pos, exp_array->getMemberType(), act_array->getMemberType());
                } else {
                    for (size_t i = 0; i < exp_array->dimensions(); ++i) {
                        if (!assertCompatible(pos, exp_array->types()[i], act_array->types()[i])) {
                            return false;
                        }
                        if (exp_array->lengths()[i] < act_array->lengths()[i]) {
                            logger_.error(pos, "type mismatch: incompatible array lengths found " +
                                               to_string(act_array->lengths()[i]) + " > " +
                                               to_string(exp_array->lengths()[i]) + ".");
                            return false;
                        }
                    }
                    return true;
                }
            } else {
                logger_.error(pos, "type mismatch: incompatible array dimensions, expected " +
                                   to_string(exp_array->dimensions()) + ", found " +
                                   to_string(act_array->dimensions()) + ".");
                return false;
            }
        } else if ((actual->isString() || actual->isChar()) &&
                (exp_array->getMemberType()->isChar() || exp_array->getMemberType()->kind() == TypeKind::ANYTYPE)) {
            if (exp_array->dimensions() == 1) {
                return true;
            } else {
                logger_.error(pos, "type mismatch: cannot assign string to multi-dimensional array.");
                return false;
            }
        }
    }
    // Check record type
    if (expected->isRecord() && actual->isRecord() && actual->extends(expected)) {
        return true;
    }
    // Check pointer type
    if (expected->isPointer()) {
        if (actual->isPointer()) {
            if (actual->extends(expected)) {
                return true;
            }
            auto exp_ptr = dynamic_cast<PointerTypeNode *>(expected);
            auto act_ptr = dynamic_cast<PointerTypeNode *>(actual);
            return assertCompatible(pos, exp_ptr->getBase(), act_ptr->getBase(), true);
        } else if (actual->kind() == TypeKind::NILTYPE) {
            return true;
        }
    }
    if (expected->isProcedure()) {
        if (actual->isProcedure()) {
            auto exp_proc = dynamic_cast<ProcedureTypeNode *>(expected);
            auto act_proc = dynamic_cast<ProcedureTypeNode *>(actual);
            if (exp_proc->hasVarArgs() || act_proc->hasVarArgs()) {
                logger_.error(pos, "procedure types with variadic arguments cannot be used here.");
                return false;
            }
            if (exp_proc->getReturnType() != act_proc->getReturnType()) {
                logger_.error(pos, "type mismatch: procedure types have different return types.");
                return false;
            }
            if (exp_proc->parameters().size() == act_proc->parameters().size()) {
                bool match = true;
                for (size_t i = 0; i < exp_proc->parameters().size(); ++i) {
                    auto exp = exp_proc->parameters()[i].get();
                    auto act = act_proc->parameters()[i].get();
                    if (exp->isVar() != act->isVar()) {
                        match = false;
                        break;
                    }
                    auto exp_param = exp->getType();
                    auto act_param = act->getType();
                    if (exp_param->isArray() && act_param->isArray()) {
                        auto exp_array = dynamic_cast<ArrayTypeNode *>(exp_param);
                        auto act_array = dynamic_cast<ArrayTypeNode *>(act_param);
                        if (!exp_array->isOpen() || !act_array->isOpen()) {
                            match = false;
                            break;
                        }
                    } else if (exp_param != act_param) {
                        match = false;
                        break;
                    }
                }
                if (!match) {
                    logger_.error(pos, "type mismatch: procedure types have different parameters.");
                    return false;
                }
                return true;
            }
            logger_.error(pos, "type mismatch: procedure types have different number of parameters.");
            return false;
        } else if (actual->kind() == TypeKind::NILTYPE) {
            return true;
        }
    }
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
        if (lhsType->isReal() && rhsType->isInteger()) {
            return lhsType;
        }
        if (lhsType->isInteger() && rhsType->isReal()) {
            return rhsType;
        }
        return lhsType->getSize() > rhsType->getSize() ? lhsType : rhsType;
    } else if ((lhsType->isChar() && rhsType->isString()) ||
               (lhsType->isString() && rhsType->isChar())) {
        // TODO This type inference rule is questionable.
        return stringTy_;
    } else if (lhsType->kind() == TypeKind::NILTYPE) {
        return rhsType;
    } else if (rhsType->kind() == TypeKind::NILTYPE) {
        return lhsType;
    }
    logger_.error(pos, "incompatible or illegal operand types (" + format(lhsType) + ", " + format(rhsType) + ")");
    return noTy_;
}

string
Sema::format(const TypeNode *type, bool isPtr) {
    std::stringstream stream;
    if (isPtr) {
        stream << "POINTER TO ";
    }
    if (type->isArray()) {
        auto array = dynamic_cast<const ArrayTypeNode *>(type);
        stream << "ARRAY";
        if (!array->isOpen()) {
            string sep = " ";
            for (unsigned length: array->lengths()) {
                stream << sep << length;
                sep = ", ";
            }
        }
        stream << " OF " << format(array->getMemberType());
    } else if (type->kind() == TypeKind::NOTYPE) {
        stream << "undefined type";
    } else {
        stream << *type;
    }
    return stream.str();
}

void Sema::cast(ExpressionNode *expr, TypeNode *expected) {
    if (expr->getType() != expected && !expected->isVirtual()) {
        expr->setCast(expected);
    }
}

void Sema::castLiteral(unique_ptr<ExpressionNode> &literal, TypeNode *expected) {
    if (expected->isVirtual()) {
        return;
    }
    auto actual = literal->getType();
    if (expected->isArray()) {
        if (actual->isChar()) {
            auto value = dynamic_cast<CharLiteralNode *>(literal.get());
            literal = onStringLiteral(literal->pos(), EMPTY_POS, string_cast(value).value());
        } else if (actual->isString()) {
            auto value = dynamic_cast<StringLiteralNode *>(literal.get())->value();
            auto array = dynamic_cast<ArrayTypeNode *>(expected);
            if (!array->isOpen() && value.size() + 1 > array->lengths()[0]) {
                logger_.warning(literal->pos(), "string literal will be truncated to length of character array.");
                literal = onStringLiteral(literal->pos(), EMPTY_POS, value.substr(0, array->lengths()[0] - 1));
            }
        }
    } else if (expected->isReal() && actual->isInteger()) {
        auto value = dynamic_cast<IntegerLiteralNode *>(literal.get());
        literal = onRealLiteral(literal->pos(), EMPTY_POS, real_cast(value).value());
        cast(literal.get(), expected);
    } else if (((expected->isInteger() || expected->isByte()) && actual->isInteger())
            || (expected->isReal() && actual->isReal())
            || ((expected->isPointer() || expected->isProcedure()) && actual->kind() == TypeKind::NILTYPE)
            || (expected->kind() == TypeKind::ANYTYPE)) {
        cast(literal.get(), expected);
    } else {
        cast(literal.get(), expected);
        logger_.warning(literal->pos(), "unable to cast " + to_string(actual) + " literal to type " + to_string(expected) + ".");
    }
}

TypeNode *
Sema::intType(int64_t value) {
    if (value >= std::numeric_limits<int16_t>::lowest() && value <= std::numeric_limits<int16_t>::max()) {
        return shortIntTy_;
    } else if (value < std::numeric_limits<int32_t>::lowest() || value > std::numeric_limits<int32_t>::max()) {
        return longIntTy_;
    }
    return integerTy_;
}
