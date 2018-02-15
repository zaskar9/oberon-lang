/*
 * Implementation of the AST Boolean constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */


#include "BooleanConstantNode.h"

BooleanConstantNode::BooleanConstantNode(bool value) : ExpressionNode(NodeType::boolean_constant), value_(value) {
}

BooleanConstantNode::~BooleanConstantNode() = default;

const bool BooleanConstantNode::getValue() const {
    return value_;
}

void BooleanConstantNode::print(std::ostream &stream) const {
    stream << (value_ ? "TRUE" : "FALSE");
}
