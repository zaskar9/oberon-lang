/*
 * Implementation of the AST Boolean constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */


#include "BooleanNode.h"

BooleanNode::BooleanNode(const FilePos pos, const bool value) :
        ValueNode(NodeType::boolean, pos, BasicTypeNode::BOOLEAN), value_(value) {
}

BooleanNode::~BooleanNode() = default;

const bool BooleanNode::getValue() const {
    return value_;
}

void BooleanNode::print(std::ostream &stream) const {
    stream << (value_ ? "TRUE" : "FALSE");
}
