/*
 * Implementation of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */


#include "ConstantNode.h"

ConstantNode::ConstantNode(const NodeType nodeType, const FilePos pos, const ExpressionType exprType) :
        ExpressionNode(nodeType, pos), exprType_(exprType) {
}

ConstantNode::~ConstantNode() = default;

bool ConstantNode::isConstant() const {
    return true;
}

ExpressionType ConstantNode::checkType() const {
    return exprType_;
}
