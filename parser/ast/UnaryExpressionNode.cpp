/*
 * Implementation of the AST unary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#include "UnaryExpressionNode.h"

UnaryExpressionNode::UnaryExpressionNode(const FilePos pos, const OperatorType op,
                                         const std::shared_ptr<const ExpressionNode> expr) :
        ExpressionNode(NodeType::unary_expression, pos), op_(op), expr_(expr) {
}

UnaryExpressionNode::~UnaryExpressionNode() = default;

bool UnaryExpressionNode::isConstant() const {
    return expr_->isConstant();
}

ExpressionType UnaryExpressionNode::checkType() const {
    return expr_->checkType();
}

const OperatorType UnaryExpressionNode::getOperator() const {
    return op_;
}

const std::shared_ptr<const ExpressionNode> UnaryExpressionNode::getExpression() const {
    return expr_;
}

void UnaryExpressionNode::print(std::ostream &stream) const {
    stream << op_;
    expr_->print(stream);
}
