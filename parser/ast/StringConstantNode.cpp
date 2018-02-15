/*
 * Implementation of the AST string constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#include "StringConstantNode.h"

StringConstantNode::StringConstantNode(const std::string &value) :
        ExpressionNode(NodeType::string_constant), value_(value) {
}

StringConstantNode::~StringConstantNode() = default;

const std::string StringConstantNode::getValue() const {
    return value_;
}

void StringConstantNode::print(std::ostream &stream) const {
    stream << value_;
}
