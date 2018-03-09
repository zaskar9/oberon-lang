/*
 * Implementation of the AST string constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#include "StringNode.h"
#include "BasicTypeNode.h"

StringNode::StringNode(const FilePos pos, const std::string &value) :
        ValueNode(NodeType::string_constant, pos, BasicTypeNode::STRING), value_(value) {
}

StringNode::~StringNode() = default;

const std::string StringNode::getValue() const {
    return value_;
}

void StringNode::print(std::ostream &stream) const {
    stream << value_;
}
