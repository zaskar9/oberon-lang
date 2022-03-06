/*
 * Semantic analysis pass used by the analyzer of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/3/20.
 */

#include <set>
#include "SemanticAnalysis.h"

void SemanticAnalysis::run(Logger *logger, Node* node) {
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
        auto symbol = symtab_->lookup(node.getName());
        if (symbol) {
            if (symbol->getNodeType() == NodeType::procedure) {
                node.resolve(dynamic_cast<ProcedureNode*>(symbol));
            } else {
                logger_->error(node.pos(), node.getName() + " is not a procedure or function.");
            }
        } else {
            logger_->error(node.pos(), "undeclared identifier: " + node.getName() + ".");
        }
    }
    if (node.isResolved()) {
        auto proc = node.dereference();
        if (node.getParameterCount() < proc->getParameterCount()) {
            logger_->error(node.pos(), "fewer actual than formal parameters.");
        }
        for (size_t cnt = 0; cnt < node.getParameterCount(); cnt++) {
            auto expr = node.getParameter(cnt);
            expr->accept(*this);
            if (cnt < proc->getParameterCount()) {
                auto parameter = proc->getParameter(cnt);
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
            } else if (!proc->hasVarArgs()) {
                logger_->error(expr->pos(), "more actual than formal parameters.");
                break;
            }
            if (expr->isConstant()) {
                node.setParameter(cnt, fold(expr));
            }
        }
    }
}

void SemanticAnalysis::visit(ModuleNode &node) {
    assertUnique(node.getName(), node);
    node.setLevel(symtab_->getLevel());
    symtab_->enterScope();
    block(node);
    symtab_->leaveScope();
}

void SemanticAnalysis::visit([[maybe_unused]] ImportNode &node) { }

void SemanticAnalysis::visit(ConstantDeclarationNode &node) {
    assertUnique(node.getName(), node);
    node.setLevel(symtab_->getLevel());
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
    assertUnique(node.getName(), node);
    node.setLevel(symtab_->getLevel());
    auto type = node.getType();
    if (type) {
        type->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined type");
    }
    node.setType(resolveType(type));
}

void SemanticAnalysis::visit(TypeReferenceNode &node) {
    auto sym = symtab_->lookup(node.getName());
    if (sym == nullptr) {
        logger_->error(node.pos(), "undefined type: " + node.getName() + ".");
    } else if (sym->getNodeType() == NodeType::array_type ||
               sym->getNodeType() == NodeType::basic_type ||
               sym->getNodeType() == NodeType::record_type) {
        node.resolve(dynamic_cast<TypeNode*>(sym));
    } else if (sym->getNodeType() == NodeType::type_declaration) {
        auto type = dynamic_cast<TypeDeclarationNode*>(sym);
        node.resolve(type->getType());
    } else {
        logger_->error(node.pos(), node.getName() + " is not a type.");
    }
}


void SemanticAnalysis::visit(VariableDeclarationNode &node) {
    assertUnique(node.getName(), node);
    node.setLevel(symtab_->getLevel());
    auto type = node.getType();
    if (type) {
        type->accept(*this);
    } else {
        logger_->error(node.pos(), "undefined variable type.");
    }
    node.setType(resolveType(type));
}

void SemanticAnalysis::visit(ProcedureNode &node) {
    assertUnique(node.getName(), node);
    node.setLevel(symtab_->getLevel());
    symtab_->enterScope();
    for (size_t i = 0; i < node.getParameterCount(); i++) {
        node.getParameter(i)->accept(*this);
    }
    auto type = node.getReturnType();
    if (type) {
        type->accept(*this);
        node.setReturnType(resolveType(type));
    }
    block(node);
    symtab_->leaveScope();
}

void SemanticAnalysis::visit(ParameterNode &node) {
    assertUnique(node.getName(), node);
    node.setLevel(symtab_->getLevel());
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
    auto sym = symtab_->lookup(node.getName());
    if (sym) {
        if (sym->getNodeType() == NodeType::constant ||
            sym->getNodeType() == NodeType::parameter ||
            sym->getNodeType() == NodeType::variable ||
            sym->getNodeType() == NodeType::procedure) {
            auto decl = dynamic_cast<DeclarationNode*>(sym);
            node.resolve(decl);
            auto type = decl->getType();
            if (sym->getNodeType() == NodeType::procedure && type == nullptr) {
                logger_->error(node.pos(), "function expected.");
                return;
            }
            for (size_t i = 0; i < node.getSelectorCount(); i++) {
                auto sel = node.getSelector(i);
                if (type->getNodeType() == NodeType::array_type) {
                    auto array_t = dynamic_cast<ArrayTypeNode*>(type);
                    sel->accept(*this);
                    if (sel->getType() != BasicTypeNode::INTEGER) {
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
                    auto record_t = dynamic_cast<RecordTypeNode*>(type);
                    if (sel->getNodeType() == NodeType::value_reference) {
                        auto ref = dynamic_cast<ValueReferenceNode*>(sel);
                        auto field = record_t->getField(ref->getName());
                        if (field) {
                            type = field->getType();
                            ref->resolve(field);
                            ref->setType(type);
                        } else {
                            logger_->error(ref->pos(), "unknown record field: " + ref->getName() + ".");
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
        logger_->error(node.pos(), "undefined identifier: " + node.getName() + ".");
    }
}



void SemanticAnalysis::visit([[maybe_unused]] BooleanLiteralNode &node) {

}

void SemanticAnalysis::visit([[maybe_unused]] IntegerLiteralNode &node) {

}

void SemanticAnalysis::visit([[maybe_unused]] StringLiteralNode &node) {

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
    } else {
        logger_->error(node.pos(), "undefined expression.");
    }
}

void SemanticAnalysis::visit(BinaryExpressionNode &node) {
    auto lhs = node.getLeftExpression();
    if (lhs) {
        lhs->accept(*this);
        if (lhs->isConstant()) {
            node.setLeftExpression(fold(lhs));
        }
    } else {
        logger_->error(node.pos(), "undefined left-hand side in expression.");
    }
    auto rhs = node.getRightExpression();
    if (rhs) {
        rhs->accept(*this);
        if (rhs->isConstant()) {
            node.setRightExpression(fold(rhs));
        }
    } else {
        logger_->error(node.pos(), "undefined right-hand side in expression.");
    }
}

void SemanticAnalysis::visit(ArrayTypeNode &node) {
    auto expr = node.getExpression();
    if (expr) {
        expr->accept(*this);
        if (expr->isConstant()) {
            if (expr->getType() == BasicTypeNode::INTEGER) {
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

void SemanticAnalysis::visit([[maybe_unused]] BasicTypeNode &node) {

}

void SemanticAnalysis::visit(RecordTypeNode &node) {
    std::set<std::string> fields;
    if (node.getFieldCount() > 0) {
        for (size_t i = 0; i < node.getFieldCount(); i++) {
            auto field = node.getField(i);
            field->accept(*this);
            if (fields.count(field->getName())) {
                logger_->error(field->pos(), "duplicate record field: " + field->getName() + ".");
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
    } else  {
        logger_->error(node.pos(), "undefined right-hand side in assignment.");
    }
}

void SemanticAnalysis::visit(IfThenElseNode &node) {
    auto condition = node.getCondition();
    if (condition) {
        condition->accept(*this);
        if (condition->getType() != BasicTypeNode::BOOLEAN) {
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
        if (condition->getType() != BasicTypeNode::BOOLEAN) {
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
        if (condition->getType() != BasicTypeNode::BOOLEAN) {
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
        if (condition->getType() != BasicTypeNode::BOOLEAN) {
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
            auto ref = dynamic_cast<ValueReferenceNode*>(counter);
            auto var = ref->dereference();
            if (var->getNodeType() != NodeType::variable) {
                logger_->error(var->pos(), "variable expected.");
            }
            if (var->getType() != BasicTypeNode::INTEGER) {
                logger_->error(var->pos(), "integer variable expected.");
            }
        } else {
            logger_->error(counter->pos(), counter->getName() + " cannot be used as a loop counter.");
        }
    } else {
        logger_->error(node.pos(), "undefined counter variable in for-loop.");
    }
    auto low = node.getLow();
    if (low) {
        low->accept(*this);
        if (low->getType() != BasicTypeNode::INTEGER) {
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
        if (high->getType() != BasicTypeNode::INTEGER) {
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
        if (expr->getType() == BasicTypeNode::INTEGER && expr->isConstant()) {
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

void SemanticAnalysis::assertUnique(const std::string &name, Node &node) {
    if (symtab_->isDuplicate(name)) {
        logger_->error(node.pos(), "duplicate definition: " + name + ".");
    }
    symtab_->insert(name, &node);
}

bool SemanticAnalysis::assertCompatible(const FilePos &pos, TypeNode *expected, TypeNode *actual) {
    if (expected == actual) {
        return true;
    } else if (expected && actual) {
        logger_->error(pos, "type mismatch: expected " + expected->getName() + ", found " + actual->getName() + ".");
        return false;
    } else {
        logger_->error(pos, "type mismatch.");
        return false;
    }
}

std::unique_ptr<LiteralNode> SemanticAnalysis::fold(const ExpressionNode *expr) const {
    auto type = expr->getType();
    if (type == BasicTypeNode::INTEGER) {
        return std::make_unique<IntegerLiteralNode>(expr->pos(), foldNumber(expr));
    } else if (type == BasicTypeNode::BOOLEAN) {
        return std::make_unique<BooleanLiteralNode>(expr->pos(), foldBoolean(expr));
    } else if (type == BasicTypeNode::STRING) {
        return std::make_unique<StringLiteralNode>(expr->pos(), foldString(expr));
    } else {
        logger_->error(expr->pos(), "incompatible types.");
        return nullptr;
    }
}

int SemanticAnalysis::foldNumber(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode*>(expr);
        int value = foldNumber(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NEG: return -1 * value;
            case OperatorType::PLUS: return value;
            default:
                logger_->error(unExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode*>(expr);
        int lValue = foldNumber(binExpr->getLeftExpression());
        int rValue = foldNumber(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:  return lValue + rValue;
            case OperatorType::MINUS: return lValue - rValue;
            case OperatorType::TIMES: return lValue * rValue;
            case OperatorType::DIV:   return lValue / rValue;
            case OperatorType::MOD:   return lValue % rValue;
            default:
                logger_->error(binExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::integer) {
        auto numConst = dynamic_cast<const IntegerLiteralNode *>(expr);
        return numConst->getValue();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode*>(ref->dereference());
            return foldNumber(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression.");
    }
    return 0;
}

bool SemanticAnalysis::foldBoolean(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode*>(expr);
        bool value = foldBoolean(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NOT: return !value;
            default:
                logger_->error(unExpr->pos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode*>(expr);
        OperatorType op = binExpr->getOperator();
        auto lhs = binExpr->getLeftExpression();
        auto rhs = binExpr->getRightExpression();
        auto type = lhs->getType();
        if (type == BasicTypeNode::BOOLEAN) {
            bool lValue = foldBoolean(binExpr->getLeftExpression());
            bool rValue = foldBoolean(binExpr->getRightExpression());
            switch (op) {
                case OperatorType::AND: return lValue && rValue;
                case OperatorType::OR:  return lValue || rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else if (type == BasicTypeNode::INTEGER) {
            int lValue = foldNumber(lhs);
            int rValue = foldNumber(rhs);
            switch (op) {
                case OperatorType::EQ:  return lValue == rValue;
                case OperatorType::NEQ: return lValue != rValue;
                case OperatorType::LT:  return lValue < rValue;
                case OperatorType::LEQ: return lValue <= rValue;
                case OperatorType::GT:  return lValue > rValue;
                case OperatorType::GEQ: return lValue >= rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else if (type == BasicTypeNode::STRING) {
            std::string lValue = foldString(lhs);
            std::string rValue = foldString(rhs);
            switch (op) {
                case OperatorType::EQ:  return lValue == rValue;
                case OperatorType::NEQ: return lValue != rValue;
                default:
                    logger_->error(binExpr->pos(), "incompatible operator.");
            }
        } else {
            logger_->error(expr->pos(), "incompatible expression.");
        }
    } else if (expr->getNodeType() == NodeType::boolean) {
        auto boolConst = dynamic_cast<const BooleanLiteralNode*>(expr);
        return boolConst->getValue();
    } else if (expr->getNodeType() == NodeType::value_reference) {
        auto ref = dynamic_cast<const ValueReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode*>(ref->dereference());
            return foldBoolean(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression.");
    }
    return false;
}

std::string SemanticAnalysis::foldString(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode*>(expr);
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
        auto ref = dynamic_cast<const ValueReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode*>(ref->dereference());
            return foldString(constant->getValue());
        }
    } else {
        logger_->error(expr->pos(), "incompatible expression (string)." );
    }
    return "";
}

TypeNode* SemanticAnalysis::resolveType(TypeNode *type) {
    if (type == nullptr) {
        return BasicTypeNode::UNDEF;
    }
    if (type->getNodeType() == NodeType::type_reference) {
        auto ref = dynamic_cast<TypeReferenceNode *>(type);
        if (ref->isResolved()) {
            return ref->dereference();
        } else {
            logger_->error(ref->pos(), "unresolved type reference: " + ref->getName() + ".");
        }
    }
    return type;
}
