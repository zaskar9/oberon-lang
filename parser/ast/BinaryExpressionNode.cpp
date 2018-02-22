/*
 * Implementation of the AST binary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#include "BinaryExpressionNode.h"

BinaryExpressionNode::BinaryExpressionNode(const FilePos pos, const OperatorType op,
                                           const std::shared_ptr<const ExpressionNode> lhs,
                                           const std::shared_ptr<const ExpressionNode> rhs) :
        ExpressionNode(NodeType::binary_expression, pos), op_(op), lhs_(lhs), rhs_(rhs) {
}

BinaryExpressionNode::~BinaryExpressionNode() = default;

bool BinaryExpressionNode::isConstant() const {
    return lhs_->isConstant() && rhs_->isConstant();
}

ExpressionType BinaryExpressionNode::checkType() const {
    ExpressionType lhsType = lhs_->checkType();
    ExpressionType rhsType = rhs_->checkType();
    if (lhsType == rhsType) {
        return lhsType;
    }
    return ExpressionType::UNDEF;
}

const OperatorType BinaryExpressionNode::getOperator() const {
    return op_;
}

const std::shared_ptr<const ExpressionNode> BinaryExpressionNode::getLeftExpression() const {
    return lhs_;
}

const std::shared_ptr<const ExpressionNode> BinaryExpressionNode::getRightExpression() const {
    return rhs_;
}

void BinaryExpressionNode::print(std::ostream &stream) const {
    lhs_->print(stream);
    stream << " ";
    stream << op_;
    stream << " ";
    rhs_->print(stream);
}

