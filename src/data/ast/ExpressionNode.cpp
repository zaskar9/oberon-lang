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
        case OperatorType::IN: result = "IN"; break;
        case OperatorType::IS: result = "IS"; break;
        case OperatorType::TIMES: result = "*"; break;
        case OperatorType::DIVIDE: result = "/"; break;
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
        case OperatorType::IN:
        case OperatorType::IS:
            return 0;
        case OperatorType::PLUS:
        case OperatorType::MINUS:
        case OperatorType::OR:
            return 1;
        case OperatorType::TIMES:
        case OperatorType::DIVIDE:
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

void ExpressionNode::setType(TypeNode *type) {
    type_ = type;
}

TypeNode *ExpressionNode::getType() const {
    return type_;
}

void ExpressionNode::setCast(TypeNode *cast) {
    cast_ = cast;
}

TypeNode *ExpressionNode::getCast() const {
    return cast_;
}


bool UnaryExpressionNode::isConstant() const {
    return expr_ && expr_->isConstant();
}

int UnaryExpressionNode::getPrecedence() const {
    return precedence(op_);
}

OperatorType UnaryExpressionNode::getOperator() const {
    return op_;
}

ExpressionNode *UnaryExpressionNode::getExpression() const {
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

int BinaryExpressionNode::getPrecedence() const {
    return precedence(op_);
}

OperatorType BinaryExpressionNode::getOperator() const {
    return op_;
}

ExpressionNode *BinaryExpressionNode::getLeftExpression() const {
    return lhs_.get();
}

ExpressionNode *BinaryExpressionNode::getRightExpression() const {
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


bool RangeExpressionNode::isConstant() const {
    return lower_ && lower_->isConstant() && upper_ && upper_->isConstant();
}

int RangeExpressionNode::getPrecedence() const {
    return 4;
}

ExpressionNode *RangeExpressionNode::getLower() const {
    return lower_.get();
}

ExpressionNode *RangeExpressionNode::getUpper() const {
    return upper_.get();
}

void RangeExpressionNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void RangeExpressionNode::print(std::ostream &stream) const {
    lower_->print(stream);
    stream << " .. ";
    upper_->print(stream);
}


bool SetExpressionNode::isConstant() const {
    for (auto &element : elements_) {
        if (!element->isConstant()) {
            return false;
        }
    }
    return true;
}

int SetExpressionNode::getPrecedence() const {
    return 4;
}

const vector<unique_ptr<ExpressionNode>> &SetExpressionNode::elements() const {
    return elements_;
}

bool SetExpressionNode::isEmptySet() const {
    return elements_.empty();
}

void SetExpressionNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void SetExpressionNode::print(std::ostream &stream) const {
    stream << "{ ";
    string sep;
    for (auto &element : elements_) {
        stream << sep;
        element->print(stream);
        sep = ", ";
    }
    stream << " }";
}

void BooleanLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void BooleanLiteralNode::print(std::ostream &stream) const {
    stream << (value() ? "TRUE" : "FALSE");
}

bool IntegerLiteralNode::isShort() const {
    return getType()->kind() == TypeKind::SHORTINT;
}

bool IntegerLiteralNode::isLong() const {
    return getType()->kind() == TypeKind::LONGINT;
}

void IntegerLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void IntegerLiteralNode::print(std::ostream &stream) const {
    stream << value();
}


bool RealLiteralNode::isLong() const {
    return getType()->kind() == TypeKind::LONGREAL;
}

void RealLiteralNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void RealLiteralNode::print(std::ostream &stream) const {
    stream << value();
}


void StringLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::print(std::ostream &stream) const {
    stream << value();
}


void CharLiteralNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void CharLiteralNode::print(std::ostream &stream) const {
    stream << value();
}


void NilLiteralNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void NilLiteralNode::print(std::ostream &stream) const {
    stream << "NIL";
}


void SetLiteralNode::print(std::ostream &stream) const {
    stream << value();
}

void SetLiteralNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}


int64_t RangeLiteralNode::lower() const {
    return lower_;
}

int64_t RangeLiteralNode::upper() const {
    return upper_;
}

void RangeLiteralNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void RangeLiteralNode::print(std::ostream &stream) const {
    stream << value();
}
