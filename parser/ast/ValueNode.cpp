/*
 * Implementation of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */


#include "ValueNode.h"

ValueNode::ValueNode(const NodeType nodeType, const FilePos pos, const ExpressionType exprType) :
        ExpressionNode(nodeType, pos), exprType_(exprType) {
}

ValueNode::~ValueNode() = default;

bool ValueNode::isConstant() const {
    return true;
}

ExpressionType ValueNode::checkType() const {
    return exprType_;
}
