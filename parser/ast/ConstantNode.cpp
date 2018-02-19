//
// Created by Michael Grossniklaus on 2/19/18.
//

#include "ConstantNode.h"

ConstantNode::ConstantNode(const NodeType nodeType, const ExpressionType exprType) :
        ExpressionNode(nodeType), exprType_(exprType) {
}

ConstantNode::~ConstantNode() = default;

bool ConstantNode::isConstant() const {
    return true;
}

ExpressionType ConstantNode::checkType() const {
    return exprType_;
}