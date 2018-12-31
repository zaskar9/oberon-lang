/*
 * Implementation of the AST number constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#include "NumberNode.h"
#include "NodeVisitor.h"

NumberNode::NumberNode(const FilePos pos, const int value) :
        ValueNode(NodeType::number, pos, BasicTypeNode::INTEGER), value_(value) {
}

NumberNode::~NumberNode() = default;

int NumberNode::getValue() const {
    return value_;
}

void NumberNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void NumberNode::print(std::ostream &stream) const {
    stream << value_;
}