/*
 * AST nodes representing unary and binary expressions in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "ExpressionNode.h"
#include "NodeVisitor.h"

std::ostream& operator<<(std::ostream &stream, const OperatorType &op) {
    std::string result;
    switch (op) {
        case OperatorType::EQ: result = "="; break;
        case OperatorType::NEQ: result = "#"; break;
        case OperatorType::LT: result = "<"; break;
        case OperatorType::GT: result = ">"; break;
        case OperatorType::GEQ: result = ">="; break;
        case OperatorType::LEQ: result = "<="; break;
        case OperatorType::TIMES: result = "*"; break;
        case OperatorType::DIV: result = "DIV"; break;
        case OperatorType::MOD: result = "MOD"; break;
        case OperatorType::PLUS: result = "+"; break;
        case OperatorType::MINUS: result = "-"; break;
        case OperatorType::AND: result = "&"; break;
        case OperatorType::OR: result = "OR"; break;
        case OperatorType::NOT: result = "~"; break;
        case OperatorType::NEG: result = "-"; break;
        default: result = "unknown operator"; break;
    }
    stream << result;
    return stream;
}

int precedence(const OperatorType &op) {
    switch (op) {
        case OperatorType::EQ:
        case OperatorType::NEQ:
        case OperatorType::LT:
        case OperatorType::GT:
        case OperatorType::GEQ:
        case OperatorType::LEQ:
            return 0;
        case OperatorType::PLUS:
        case OperatorType::MINUS:
        case OperatorType::OR:
            return 1;
        case OperatorType::TIMES:
        case OperatorType::DIV:
        case OperatorType::MOD:
        case OperatorType::AND:
            return 2;
        case OperatorType::NOT:
        case OperatorType::NEG:
            return 3;
        default:
            return 4;
    }
}


ExpressionNode::~ExpressionNode() = default;


bool UnaryExpressionNode::isConstant() const {
    return expr_->isConstant();
}

TypeNode* UnaryExpressionNode::getType() const {
    return expr_->getType();
}

int UnaryExpressionNode::getPrecedence() const {
    return precedence(op_);
}

OperatorType UnaryExpressionNode::getOperator() const {
    return op_;
}

void UnaryExpressionNode::setExpression(std::unique_ptr<ExpressionNode> expr) {
    expr_ = std::move(expr);
}

ExpressionNode* UnaryExpressionNode::getExpression() const {
    return expr_.get();
}

void UnaryExpressionNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void UnaryExpressionNode::print(std::ostream &stream) const {
    stream << op_;
    expr_->print(stream);
}


bool BinaryExpressionNode::isConstant() const {
    return lhs_ && lhs_->isConstant() && rhs_ && rhs_->isConstant();
}

TypeNode* BinaryExpressionNode::getType() const {
    if (lhs_ && rhs_) {
        auto lhsType = lhs_->getType();
        auto rhsType = rhs_->getType();
        if (lhsType == rhsType) {
            if (op_==OperatorType::EQ
                || op_==OperatorType::NEQ
                || op_==OperatorType::LT
                || op_==OperatorType::LEQ
                || op_==OperatorType::GT
                || op_==OperatorType::GEQ) {
                return BasicTypeNode::BOOLEAN;
            }
            return lhsType;
        }
        return nullptr;
    }
    return nullptr;
}

int BinaryExpressionNode::getPrecedence() const {
    return precedence(op_);
}

OperatorType BinaryExpressionNode::getOperator() const {
    return op_;
}

void BinaryExpressionNode::setLeftExpression(std::unique_ptr<ExpressionNode> expr) {
    lhs_ = std::move(expr);
}

ExpressionNode* BinaryExpressionNode::getLeftExpression() const {
    return lhs_.get();
}

void BinaryExpressionNode::setRightExpression(std::unique_ptr<ExpressionNode> expr) {
    rhs_ = std::move(expr);
}

ExpressionNode* BinaryExpressionNode::getRightExpression() const {
    return rhs_.get();
}

void BinaryExpressionNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void BinaryExpressionNode::print(std::ostream &stream) const {
    lhs_->print(stream);
    stream << " ";
    stream << op_;
    stream << " ";
    rhs_->print(stream);
}

bool LiteralNode::isConstant() const {
    return true;
}

BasicTypeNode* LiteralNode::getType() const {
    return type_;
}

int LiteralNode::getPrecedence() const {
    return 4;
}

bool BooleanLiteralNode::getValue() const {
    return value_;
}

void BooleanLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void BooleanLiteralNode::print(std::ostream &stream) const {
    stream << (value_ ? "TRUE" : "FALSE");
}


int IntegerLiteralNode::getValue() const {
    return value_;
}

void IntegerLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void IntegerLiteralNode::print(std::ostream &stream) const {
    stream << value_;
}


std::string StringLiteralNode::getValue() const {
    return value_;
}

void StringLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::print(std::ostream &stream) const {
    stream << value_;
}
