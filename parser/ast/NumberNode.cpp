/*
 * Implementation of the AST number constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#include "NumberNode.h"

NumberNode::NumberNode(const FilePos pos, const int value) :
        ValueNode(NodeType::number, pos, BasicTypeNode::INTEGER), value_(value) {
}

NumberNode::~NumberNode() = default;

const int NumberNode::getValue() const {
    return value_;
}

void NumberNode::print(std::ostream &stream) const {
    stream << value_;
}