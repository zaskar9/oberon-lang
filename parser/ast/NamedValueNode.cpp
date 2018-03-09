/*
 * Implementation of the AST named-value nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "NamedValueNode.h"

NamedValueNode::NamedValueNode(NodeType nodeType, FilePos pos, const std::string &name,
                               const std::shared_ptr<const TypeNode> &type) :
        Node(nodeType, pos), name_(name), type_(type) {
}

NamedValueNode::~NamedValueNode() = default;

const std::string NamedValueNode::getName() const {
    return name_;
}

std::shared_ptr<const TypeNode> NamedValueNode::getType() const {
    return type_;
}

void NamedValueNode::print(std::ostream &stream) const {
    stream << name_ << ": " << *type_;
}