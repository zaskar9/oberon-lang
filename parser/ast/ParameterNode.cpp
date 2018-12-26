/*
 * Implementation of the AST parameter declaration nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ParameterNode.h"

ParameterNode::ParameterNode(FilePos pos, const std::string &name, const TypeNode *type, bool var) :
        NamedValueNode(NodeType::parameter, pos, name, type), var_(var) {
}

ParameterNode::~ParameterNode() = default;

const bool ParameterNode::isVar() const {
    return var_;
}

void ParameterNode::print(std::ostream &stream) const {
    stream << (var_ ? "VAR " : "") << this->getName() << ": " << *this->getType();
}