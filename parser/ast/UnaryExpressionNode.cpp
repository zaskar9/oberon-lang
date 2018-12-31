/*
 * Implementation of the AST unary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#include "UnaryExpressionNode.h"
#include "NodeVisitor.h"

UnaryExpressionNode::UnaryExpressionNode(const FilePos pos, const OperatorType op, std::unique_ptr<ExpressionNode> expr) :
        ExpressionNode(NodeType::unary_expression, pos), op_(op), expr_(std::move(expr)) {
}

UnaryExpressionNode::~UnaryExpressionNode() = default;

bool UnaryExpressionNode::isConstant() const {
    return expr_->isConstant();
}

TypeNode* UnaryExpressionNode::getType() const {
    return expr_->getType();
}

OperatorType UnaryExpressionNode::getOperator() const {
    return op_;
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
