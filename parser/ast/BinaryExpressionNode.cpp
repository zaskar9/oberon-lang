/*
 * Implementation of the AST binary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#include "BinaryExpressionNode.h"

BinaryExpressionNode::BinaryExpressionNode(const FilePos pos, const OperatorType op,
                                           std::unique_ptr<const ExpressionNode> lhs,
                                           std::unique_ptr<const ExpressionNode> rhs) :
        ExpressionNode(NodeType::binary_expression, pos), op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
}

BinaryExpressionNode::~BinaryExpressionNode() = default;

bool BinaryExpressionNode::isConstant() const {
    return lhs_->isConstant() && rhs_->isConstant();
}

ExpressionType BinaryExpressionNode::checkType() const {
    ExpressionType lhsType = lhs_->checkType();
    ExpressionType rhsType = rhs_->checkType();
    if (lhsType == rhsType) {
        if (op_ == OperatorType::EQ
            || op_ == OperatorType::NEQ
            || op_ == OperatorType::LT
            || op_ == OperatorType::LEQ
            || op_ == OperatorType::GT
            || op_ == OperatorType::GEQ) {
            return ExpressionType::BOOLEAN;
        }
        return lhsType;
    }
    return ExpressionType::UNDEF;
}

const OperatorType BinaryExpressionNode::getOperator() const {
    return op_;
}

const ExpressionNode* BinaryExpressionNode::getLeftExpression() const {
    return lhs_.get();
}

const ExpressionNode* BinaryExpressionNode::getRightExpression() const {
    return rhs_.get();
}

void BinaryExpressionNode::print(std::ostream &stream) const {
    lhs_->print(stream);
    stream << " ";
    stream << op_;
    stream << " ";
    rhs_->print(stream);
}

