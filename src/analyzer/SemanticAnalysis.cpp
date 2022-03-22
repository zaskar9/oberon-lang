/*
 * Semantic analysis pass used by the analyzer of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/3/20.
 */

#include <set>
#include "SemanticAnalysis.h"

SemanticAnalysis::SemanticAnalysis(SymbolTable *symbols, SymbolImporter *importer, SymbolExporter *exporter)
        : Analysis(), NodeVisitor(),
          symbols_(symbols), logger_(), parent_(), importer_(importer), exporter_(exporter) {
    tBoolean_ = dynamic_cast<TypeNode *>(symbols_->lookup(SymbolTable::BOOLEAN));
    tByte_ = dynamic_cast<TypeNode *>(symbols_->lookup(SymbolTable::BYTE));
    tChar_ = dynamic_cast<TypeNode *>(symbols_->lookup(SymbolTable::CHAR));
    tInteger_ = dynamic_cast<TypeNode *>(symbols_->lookup(SymbolTable::INTEGER));
    tReal_ = dynamic_cast<TypeNode *>(symbols_->lookup(SymbolTable::REAL));
    tString_ = dynamic_cast<TypeNode *>(symbols_->lookup(SymbolTable::STRING));
}

void SemanticAnalysis::run(Logger *logger, Node *node) {
    logger_ = logger;
    node->accept(*this);
}

void SemanticAnalysis::block(BlockNode &node) {
    auto parent = parent_;
    parent_ = &node;
    for (size_t cnt = 0; cnt < node.getConstantCount(); cnt++) {
        node.getConstant(cnt)->accept(*this);
    }
    for (size_t cnt = 0; cnt < node.getTypeDeclarationCount(); cnt++) {
        node.getTypeDeclaration(cnt)->accept(*this);
    }
    for (size_t cnt = 0; cnt < node.getVariableCount(); cnt++) {
        node.getVariable(cnt)->accept(*this);
    }
    for (size_t cnt = 0; cnt < node.getProcedureCount(); cnt++) {
        node.getProcedure(cnt)->accept(*this);
    }
    node.getStatements()->accept(*this);
    parent_ = parent;
}

void SemanticAnalysis::call(ProcedureNodeReference &node) {
    if (!node.isResolved()) {
        auto symbol = symbols_->lookup(node.getIdentifier());
        if (symbol) {
            if (symbol->getNodeType() == NodeType::procedure) {
                auto proc = dynamic_cast<ProcedureNode *>(symbol);
                node.resolve(proc);
                if (node.getIdentifier()->isQualified() && proc->isExtern()) {
                    // a fully-qualified external reference needs to added to module for code generation
                    module_->addExternalProcedure(proc);
                }
            } else {
                logger_->error(node.pos(), to_string(*node.getIdentifier()) + " is not a procedure or function.");
            }
        } else {
            logger_->error(node.pos(), "undeclared identifier: " + to_string(*node.getIdentifier()) + ".");
        }
    }
    if (node.isResolved()) {
        auto proc = node.dereference();
        if (node.getActualParameterCount() < proc->getFormalParameterCount()) {
            logger_->error(node.pos(), "fewer actual than formal parameters.");
        }
        for (size_t cnt = 0; cnt < node.getActualParameterCount(); cnt++) {
            auto expr = node.getActualParameter(cnt);
            expr->accept(*this);
            if (cnt < proc->getFormalParameterCount()) {
                auto parameter = proc->getFormalParameter(cnt);
                if (assertCompatible(expr->pos(), parameter->getType(), expr->getType())) {
                    if (parameter->isVar()) {
                        if (expr->getNodeType() == NodeType::value_reference) {
                            auto reference = dynamic_cast<const ValueReferenceNode *>(expr);
                            auto value = reference->dereference();
                            if (value->getNodeType() == NodeType::constant) {
                                logger_->error(expr->pos(),
                                               "illegal actual parameter: cannot pass constant by reference.");
                            }
                        } else {
                            logger_->error(expr->pos(),
                                           "illegal actual parameter: cannot pass expression by reference.");
                        }
                    }
                }
                // Casting
                if (parameter->getType() != expr->getType()) {
                    expr->setCast(parameter->getType());
                }
            } else if (!proc->hasVarArgs()) {
                logger_->error(expr->pos(), "more actual than formal parameters.");
                break;
            }
            // Folding
            if (expr->isConstant()) {
                node.setActualParameter(cnt, fold(expr));
            }
        }
    }
}

void SemanticAnalysis::visit(ModuleNode &node) {
    auto name = node.getIdentifier()->name();
    symbols_->createNamespace(name, true);
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    module_ = &node;
    for (size_t i = 0; i < node.getImportCount(); i++) {
        node.getImport(i)->accept(*this);
    }
    symbols_->openScope();
    block(node);
    symbols_->closeScope();
    if (logger_->getErrorCount() == 0) {
        exporter_->write(name, symbols_);
    }
}

void SemanticAnalysis::visit(ImportNode &node) {
    // TODO check duplicate imports
    std::unique_ptr<ModuleNode> module;
    if (node.getAlias()) {
        module = importer_->read(node.getAlias()->name(), node.getModule()->name(), symbols_);
    } else {
        module = importer_->read(node.getModule()->name(), symbols_);
    }
    if (module) {
        module_->addExternalModule(std::move(module));
    } else {
        logger_->error(node.pos(), "Module " + node.getModule()->name() + " could not be imported.");
    }
}

void SemanticAnalysis::visit(ConstantDeclarationNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    auto expr = node.getValue();
    if (expr) {
        expr->accept(*this);
        auto value = fold(expr);
        if (value) {
            if (value->isConstant()) {
                node.setType(value->getType());
                node.setValue(std::move(value));
            } else {
                logger_->error(value->pos(), "value must be constant.");
            }
        } else {
            logger_->error(node.pos(), "undefined constant.");
        }
    } else {
        logger_->error(node.pos(), "undefined constant.");
    }
}

void SemanticAnalysis::visit(TypeDeclarationNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    auto type = node.getType();
    if (type) {
        type->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined type");
    }
    node.setType(resolveType(type));
}

void SemanticAnalysis::visit(TypeReferenceNode &node) {
    auto sym = symbols_->lookup(node.getIdentifier());
    if (sym == nullptr) {
        logger_->error(node.pos(), "undefined type: " + to_string(*node.getIdentifier()) + ".");
    } else if (sym->getNodeType() == NodeType::array_type ||
               sym->getNodeType() == NodeType::basic_type ||
               sym->getNodeType() == NodeType::record_type) {
        node.resolve(dynamic_cast<TypeNode *>(sym));
    } else if (sym->getNodeType() == NodeType::type_declaration) {
        auto type = dynamic_cast<TypeDeclarationNode *>(sym);
        node.resolve(type->getType());
    } else {
        logger_->error(node.pos(), to_string(*node.getIdentifier()) + " is not a type.");
    }
}


void SemanticAnalysis::visit(VariableDeclarationNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    auto type = node.getType();
    if (type) {
        type->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined variable type.");
    }
    node.setType(resolveType(type));
}

void SemanticAnalysis::visit(ProcedureNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    checkExport(node);
    symbols_->openScope();
    for (size_t i = 0; i < node.getFormalParameterCount(); i++) {
        node.getFormalParameter(i)->accept(*this);
    }
    auto type = node.getReturnType();
    if (type) {
        type->accept(*this);
        node.setReturnType(resolveType(type));
    }
    block(node);
    symbols_->closeScope();
}

void SemanticAnalysis::visit(ParameterNode &node) {
    assertUnique(node.getIdentifier(), node);
    node.setLevel(symbols_->getLevel());
    auto type = node.getType();
    if (type) {
        type->accept(*this);
        node.setType(resolveType(type));
    } else {
        logger_->error(node.pos(), "undefined parameter type.");
    }
}

void SemanticAnalysis::visit(FieldNode &node) {
    auto type = node.getType();
    if (type) {
        type->accept(*this);
        node.setType(resolveType(type));
    } else {
        logger_->error(node.pos(), "undefined record field type.");
    }
}

void SemanticAnalysis::visit(ValueReferenceNode &node) {
    auto ident = node.getIdentifier();
    auto sym = symbols_->lookup(ident);
    if (!sym && ident->isQualified()) {
        sym = symbols_->lookup(ident->qualifier());
        auto pos = node.pos();
        pos.charNo += ident->qualifier().size() + 1;
        node.insertSelector(0, NodeType::record_type,
                            std::make_unique<ValueReferenceNode>(pos,std::make_unique<Identifier>(pos, ident->name())));
    }
    if (sym) {
        if (sym->getNodeType() == NodeType::constant ||
            sym->getNodeType() == NodeType::parameter ||
            sym->getNodeType() == NodeType::variable ||
            sym->getNodeType() == NodeType::procedure) {
            auto decl = dynamic_cast<DeclarationNode *>(sym);
            node.resolve(decl);
            auto type = decl->getType();
            if (!type) {
                if (sym->getNodeType() == NodeType::procedure) {
                    logger_->error(node.pos(), "function expected.");
                    return;
                }
                logger_->error(node.pos(), "type undefined for " + to_string(*node.getIdentifier()) + ".");
                return;
            }
            for (size_t i = 0; i < node.getSelectorCount(); i++) {
                auto sel = node.getSelector(i);
                if (type->getNodeType() == NodeType::array_type) {
                    auto array_t = dynamic_cast<ArrayTypeNode *>(type);
                    sel->accept(*this);
                    auto sel_type = sel->getType();
                    if (sel_type && sel_type->kind() != TypeKind::INTEGER) {
                        logger_->error(sel->pos(), "integer expression expected.");
                    }
                    if (sel->isConstant()) {
                        auto value = fold(sel);
                        if (value) {
                            node.setSelector(i, std::move(value));
                        }
                    }
                    type = array_t->getMemberType();
                } else if (type->getNodeType() == NodeType::record_type) {
                    auto record_t = dynamic_cast<RecordTypeNode *>(type);
                    if (sel->getNodeType() == NodeType::value_reference) {
                        auto ref = dynamic_cast<ValueReferenceNode *>(sel);
                        auto field = record_t->getField(ref->getIdentifier()->name());
                        if (field) {
                            type = field->getType();
                            ref->resolve(field);
                            ref->setType(type);
                        } else {
                            logger_->error(ref->pos(),
                                           "unknown record field: " + to_string(*ref->getIdentifier()) + ".");
                        }
                    } else {
                        logger_->error(sel->pos(), "identifier expected.");
                    }
                } else {
                    logger_->error(sel->pos(), "unexpected selector.");
                }
            }
            node.setType(resolveType(type));
        } else {
            logger_->error(node.pos(), "constant, parameter, variable, function call expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined identifier: " + to_string(*node.getIdentifier()) + ".");
    }
}


void SemanticAnalysis::visit(BooleanLiteralNode &node) {
    node.setType(this->tBoolean_);
}

void SemanticAnalysis::visit(IntegerLiteralNode &node) {
    node.setType(this->tInteger_);
}

void SemanticAnalysis::visit(StringLiteralNode &node) {
    node.setType(this->tString_);
}

void SemanticAnalysis::visit(FunctionCallNode &node) {
    call(node);
}

void SemanticAnalysis::visit(UnaryExpressionNode &node) {
    auto expr = node.getExpression();
    if (expr) {
        expr->accept(*this);
        if (expr->isConstant()) {
            auto value = fold(expr);
            if (value) {
                node.setExpression(std::move(value));
            }
        }
        node.setType(node.getExpression()->getType());
    } else {
        logger_->error(node.pos(), "undefined expression.");
    }
}

void SemanticAnalysis::visit(BinaryExpressionNode &node) {
    auto lhs = node.getLeftExpression();
    if (lhs) {
        lhs->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined left-hand side in expression.");
    }
    auto rhs = node.getRightExpression();
    if (rhs) {
        rhs->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined right-hand side in expression.");
    }
    if (lhs && rhs) {
        auto op = node.getOperator();
        // Type inference
        auto lhsType = lhs->getType();
        auto rhsType = rhs->getType();
        auto common = commonType(lhsType, rhsType);
        if (common) {
            if (op == OperatorType::EQ
                || op == OperatorType::NEQ
                || op == OperatorType::LT
                || op == OperatorType::LEQ
                || op == OperatorType::GT
                || op == OperatorType::GEQ) {
                node.setType(this->tBoolean_);
            } else {
                node.setType(common);
            }
        } else {
            logger_->error(node.pos(), "incompatible types.");
        }
        // Folding
        if (lhs->isConstant()) {
            node.setLeftExpression(fold(lhs));
        }
        if (rhs->isConstant()) {
            node.setRightExpression(fold(rhs));
        }
        // Casting
        if (common) {
            if (node.getLeftExpression()->getType() != common) {
                node.getLeftExpression()->setCast(common);
            }
            if (node.getRightExpression()->getType() != common) {
                node.getRightExpression()->setCast(common);
            }
        }
    }
}

void SemanticAnalysis::visit(ArrayTypeNode &node) {
    auto expr = node.getExpression();
    if (expr) {
        expr->accept(*this);
        if (expr->isConstant()) {
            auto type = expr->getType();
            if (type && type->kind() == TypeKind::INTEGER) {
                auto dim = foldNumber(expr);
                if (dim > 0) {
                    node.setDimension((unsigned int) dim);
                } else {
                    logger_->error(expr->pos(), "array dimension must be a positive value.");
                }
            } else {
                logger_->error(expr->pos(), "integer expression expected.");
            }
        } else {
            logger_->error(expr->pos(), "constant expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined array type.");
    }
    auto type = node.getMemberType();
    if (type) {
        type->accept(*this);
        node.setMemberType(resolveType(type));
    } else {
        logger_->error(node.pos(), "undefined type");
    }
    if (type && type->getSize() > 0 && node.getDimension() > 0) {
        node.setSize(node.getDimension() * type->getSize());
    } else {
        logger_->error(node.pos(), "undefined array dimension.");
    }
}

void SemanticAnalysis::visit([[maybe_unused]] BasicTypeNode &node) {}

void SemanticAnalysis::visit([[maybe_unused]] ProcedureTypeNode &node) {}

void SemanticAnalysis::visit(RecordTypeNode &node) {
    std::set<std::string> fields;
    if (node.getFieldCount() > 0) {
        for (size_t i = 0; i < node.getFieldCount(); i++) {
            auto field = node.getField(i);
            if (field->getIdentifier()->isExported()) {
                if (symbols_->getLevel() != SymbolTable::MODULE_LEVEL) {
                    logger_->error(field->pos(), "only top-level declarations can be exported.");
                } else if (!node.getIdentifier()->isExported()) {
                    logger_->error(field->pos(), "cannot export fields of non-exported record type.");
                }
            }
            field->accept(*this);
            if (fields.count(field->getIdentifier()->name())) {
                logger_->error(field->pos(), "duplicate record field: " + to_string(*field->getIdentifier()) + ".");
            }
        }
    } else {
        logger_->error(node.pos(), "records needs at least one field.");
    }
}

void SemanticAnalysis::visit(StatementSequenceNode &node) {
    for (size_t i = 0; i < node.getStatementCount(); i++) {
        auto statement = node.getStatement(i);
        if (statement) {
            node.getStatement(i)->accept(*this);
        }
    }
}

void SemanticAnalysis::visit(AssignmentNode &node) {
    auto lvalue = node.getLvalue();
    if (lvalue) {
        lvalue->accept(*this);
        auto decl = lvalue->dereference();
        if (decl) {
            if (decl->getNodeType() == NodeType::parameter) {
                auto param = dynamic_cast<ParameterNode *>(lvalue->dereference());
                if (!param->isVar()) {
                    logger_->error(lvalue->pos(), "cannot assign non-var parameter.");
                }
            } else if (lvalue->dereference()->getNodeType() == NodeType::constant) {
                logger_->error(lvalue->pos(), "cannot assign constant.");
            }
        } else {
            logger_->error(node.pos(), "undefined left-hand side in assignment.");
        }
    } else {
        logger_->error(node.pos(), "undefined left-hand side in assignment.");
    }
    auto rvalue = node.getRvalue();
    if (rvalue) {
        rvalue->accept(*this);
        if (lvalue) {
            assertCompatible(lvalue->pos(), lvalue->getType(), rvalue->getType());
        }
        if (rvalue->isConstant()) {
            node.setRvalue(fold(rvalue));
        }
    } else {
        logger_->error(node.pos(), "undefined right-hand side in assignment.");
    }
}

void SemanticAnalysis::visit(IfThenElseNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in if-statement.");
    }
    node.getThenStatements()->accept(*this);
    if (node.hasElseIf()) {
        for (size_t i = 0; i < node.getElseIfCount(); i++) {
            node.getElseIf(i)->accept(*this);
        }
    }
    if (node.hasElse()) {
        node.getElseStatements()->accept(*this);
    }
}

void SemanticAnalysis::visit(ElseIfNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in elsif-statement.");
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(ProcedureCallNode &node) {
    call(node);
}

void SemanticAnalysis::visit(LoopNode &node) {
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(WhileLoopNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in while-loop.");
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(RepeatLoopNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        auto type = condition->getType();
        if (type && type->kind() != TypeKind::BOOLEAN) {
            logger_->error(condition->pos(), "Boolean expression expected.");
        }
    } else {
        logger_->error(node.pos(), "undefined condition in repeat-loop.");
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(ForLoopNode &node) {
    auto counter = node.getCounter();
    if (counter) {
        counter->accept(*this);
        if (counter->getNodeType() == NodeType::value_reference) {
            auto ref = dynamic_cast<ValueReferenceNode *>(counter);
            auto var = ref->dereference();
            if (var->getNodeType() != NodeType::variable) {
                logger_->error(var->pos(), "variable expected.");
            }
            auto type = var->getType();
            if (type && type->kind() != TypeKind::INTEGER) {
                logger_->error(var->pos(), "integer variable expected.");
            }
        } else {
            logger_->error(counter->pos(), to_string(*counter->getIdentifier()) + " cannot be used as a loop counter.");
        }
    } else {
        logger_->error(node.pos(), "undefined counter variable in for-loop.");
    }
    auto low = node.getLow();
    if (low) {
        low->accept(*this);
        auto type = low->getType();
        if (type && type->kind() != TypeKind::INTEGER) {
            logger_->error(low->pos(), "integer expression expected.");
        }
        if (low->isConstant()) {
            node.setLow(fold(low));
        }
    } else {
        logger_->error(node.pos(), "undefined low value in for-loop.");
    }
    auto high = node.getHigh();
    if (high) {
        high->accept(*this);
        auto type = high->getType();
        if (type && type->kind() != TypeKind::INTEGER) {
            logger_->error(high->pos(), "integer expression expected.");
        }
        if (high->isConstant()) {
            node.setHigh(fold(high));
        }
    } else {
        logger_->error(node.pos(), "undefined high value in for-loop.");
    }
    auto expr = node.getStep();
    if (expr) {
        expr->accept(*this);
        auto type = expr->getType();
        if (type && type->kind() == TypeKind::INTEGER && expr->isConstant()) {
            auto step = foldNumber(expr);
            if (step == 0) {
                logger_->error(expr->pos(), "step value cannot be zero.");
            }
            node.setStep(std::make_unique<IntegerLiteralNode>(expr->pos(), step));
        } else {
            logger_->error(expr->pos(), "constant integer expression expected.");
        }
    }
    node.getStatements()->accept(*this);
}

void SemanticAnalysis::visit(ReturnNode &node) {
    auto expr = node.getValue();
    if (expr) {
        expr->accept(*this);
    }
    if (parent_->getNodeType() == NodeType::module) {
        if (expr) {
            logger_->error(expr->pos(), "module cannot return a value.");
        }
    }
    if (parent_->getNodeType() == NodeType::procedure) {
        auto proc = dynamic_cast<ProcedureNode *>(parent_);
        if (expr) {
            if (!proc->getReturnType()) {
                logger_->error(expr->pos(), "procedure cannot return a value.");
            } else {
                assertCompatible(expr->pos(), proc->getReturnType(), expr->getType());
            }
        } else {
            if (proc->getReturnType()) {
                logger_->error(node.pos(), "function must return value.");
            }
        }
    }
}

void SemanticAnalysis::assertUnique(Identifier *ident, Node &node) {
    if (ident->isQualified()) {
        logger_->error(ident->pos(), "cannot use qualified identifier here.");
    }
    if (symbols_->isDuplicate(ident->name())) {
        logger_->error(ident->pos(), "duplicate definition: " + ident->name() + ".");
    }
    symbols_->insert(ident->name(), &node);
}

bool SemanticAnalysis::assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual) {
    if (expected == actual) {
        return true;
    } else if (expected && actual) {
        auto expectedId = expected->getIdentifier();
        auto actualId = actual->getIdentifier();
        if (expectedId->name() == actualId->name() && expectedId->qualifier() == actualId->qualifier()) {
            return true;
        } else if (isInteger(expected) && isInteger(actual)) {
            if (expected->getSize() < actual->getSize()) {
                logger_->error(pos, "type mismatch: casting " + to_string(*actualId) +
                                    " down to " + to_string(*expectedId) + " loses data.");
                return false;
            }
            return true;
        }
        logger_->error(pos, "type mismatch: expected " + to_string(*expectedId) +
                            ", found " + to_string(*actualId) + ".");
        return false;
    } else {
        logger_->error(pos, "type mismatch.");
        return false;
    }
}

void SemanticAnalysis::checkExport(DeclarationNode &node) {
    if (node.getLevel() != SymbolTable::MODULE_LEVEL && node.getIdentifier()->isExported()) {
        logger_->error(node.getIdentifier()->pos(), "only top-level declarations can be exported.");
    }
}

std::unique_ptr<LiteralNode> SemanticAnalysis::fold(const ExpressionNode *expr) const {
    auto type = expr->getType();
    if (type) {
        auto cast = expr->getCast();
        if (type->kind() == TypeKind::INTEGER) {
            return std::make_unique<IntegerLiteralNode>(expr->pos(), foldNumber(expr), type, cast);
        } else if (type->kind() == TypeKind::BOOLEAN) {
            return std::make_unique<BooleanLiteralNode>(expr->pos(), foldBoolean(expr), type, cast);
        } else if (type->kind() == TypeKind::STRING) {
            return std::make_unique<StringLiteralNode>(expr->pos(), foldString(expr), type, cast);
        }
        logger_->error(expr->pos(), "incompatible types.");
    } else {
        logger_->error(expr->pos(), "undefined type.");
    }
    return nullptr;
}

int SemanticAnalysis::foldNumber(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode *>(expr);
        int value = foldNumber(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NEG:
                return -1 * value;
            case OperatorType::PLUS:
                return value;
            default:
                logger_->error(unExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode *>(expr);
        int lValue = foldNumber(binExpr->getLeftExpression());
        int rValue = foldNumber(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:
                return lValue + rValue;
            case OperatorType::MINUS:
                return lValue - rValue;
            case OperatorType::TIMES:
                return lValue * rValue;
            case OperatorType::DIV:
                return lValue / rValue;
            case OperatorType::MOD:
                return lValue % rValue;
            default:
                logger_->error(binExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::integer) {
        auto numConst = dynamic_cast<const IntegerLiteralNode *>(expr);
        return numConst->getValue();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return foldNumber(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression.");
    }
    return 0;
}

bool SemanticAnalysis::foldBoolean(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode *>(expr);
        bool value = foldBoolean(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NOT:
                return !value;
            default:
                logger_->error(unExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode *>(expr);
        OperatorType op = binExpr->getOperator();
        auto lhs = binExpr->getLeftExpression();
        auto rhs = binExpr->getRightExpression();
        auto type = lhs->getType();
        if (type->kind() == TypeKind::BOOLEAN) {
            bool lValue = foldBoolean(binExpr->getLeftExpression());
            bool rValue = foldBoolean(binExpr->getRightExpression());
            switch (op) {
                case OperatorType::AND:
                    return lValue && rValue;
                case OperatorType::OR:
                    return lValue || rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else if (type->kind() == TypeKind::INTEGER) {
            int lValue = foldNumber(lhs);
            int rValue = foldNumber(rhs);
            switch (op) {
                case OperatorType::EQ:
                    return lValue == rValue;
                case OperatorType::NEQ:
                    return lValue != rValue;
                case OperatorType::LT:
                    return lValue < rValue;
                case OperatorType::LEQ:
                    return lValue <= rValue;
                case OperatorType::GT:
                    return lValue > rValue;
                case OperatorType::GEQ:
                    return lValue >= rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else if (type->kind() == TypeKind::STRING) {
            std::string lValue = foldString(lhs);
            std::string rValue = foldString(rhs);
            switch (op) {
                case OperatorType::EQ:
                    return lValue == rValue;
                case OperatorType::NEQ:
                    return lValue != rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else {
            logger_->error(expr->pos(), "incompatible expression.");
        }
    } else if (expr->getNodeType() == NodeType::boolean) {
        auto boolConst = dynamic_cast<const BooleanLiteralNode *>(expr);
        return boolConst->getValue();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return foldBoolean(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression.");
    }
    return false;
}

std::string SemanticAnalysis::foldString(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode *>(expr);
        std::string lValue = foldString(binExpr->getLeftExpression());
        std::string rValue = foldString(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:
                return std::string(lValue + rValue);
            default:
                logger_->error(binExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::string) {
        auto stringConst = dynamic_cast<const StringLiteralNode *>(expr);
        return stringConst->getValue();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode *>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode *>(ref->dereference());
            return foldString(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression (string).");
    }
    return "";
}

bool SemanticAnalysis::isNumeric(TypeNode *type) const {
    switch (type->kind()) {
        case TypeKind::BYTE:
        case TypeKind::CHAR:
        case TypeKind::INTEGER:
        case TypeKind::LONGINT:
        case TypeKind::REAL:
        case TypeKind::LONGREAL:
            return true;
        default:
            return false;
    }
}

bool SemanticAnalysis::isInteger(TypeNode *type) const {
    switch (type->kind()) {
        case TypeKind::BYTE:
        case TypeKind::CHAR:
        case TypeKind::INTEGER:
        case TypeKind::LONGINT:
            return true;
        default:
            return false;
    }
}

TypeNode *SemanticAnalysis::commonType(TypeNode *lhsType, TypeNode *rhsType) const {
    if (lhsType == rhsType) {
        return lhsType;
    } else if (lhsType->getIdentifier()->name() == rhsType->getIdentifier()->name() &&
               lhsType->getIdentifier()->qualifier() == rhsType->getIdentifier()->qualifier()) {
        return lhsType;
    } else if (isNumeric(lhsType) && isNumeric(rhsType)) {
        if (isInteger(lhsType) && isInteger(rhsType)) {
            return lhsType->getSize() > rhsType->getSize() ? lhsType : rhsType;
        }
    }
    return nullptr;
}

TypeNode *SemanticAnalysis::resolveType(TypeNode *type) {
    if (type == nullptr) {
        return type;
    }
    if (type->getNodeType() == NodeType::type_reference) {
        auto ref = dynamic_cast<TypeReferenceNode *>(type);
        if (ref->isResolved()) {
            return ref->dereference();
        } else {
            logger_->error(ref->pos(), "unresolved type reference: " + to_string(*ref->getIdentifier()) + ".");
        }
    }
    return type;
}