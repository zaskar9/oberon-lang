/*
 * Implementation of the AST number constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#include "NumberConstantNode.h"

NumberConstantNode::NumberConstantNode(int value) :
        ConstantNode(NodeType::number_constant, ExpressionType::INTEGER), value_(value) {
}

NumberConstantNode::~NumberConstantNode() = default;

const int NumberConstantNode::getValue() const {
    return value_;
}

void NumberConstantNode::print(std::ostream &stream) const {
    stream << value_;
}