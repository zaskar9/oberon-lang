/*
 * Implementation of the AST named-value nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "NamedValueNode.h"
#include "TypeReferenceNode.h"

NamedValueNode::NamedValueNode(NodeType nodeType, const FilePos pos, const std::string &name, const TypeNode* type) :
        Node(nodeType, pos), name_(name), type_(type) {
}

NamedValueNode::~NamedValueNode() = default;

const std::string NamedValueNode::getName() const {
    return name_;
}

const TypeNode* NamedValueNode::getType() const {
    if (type_->getNodeType() == NodeType::type_reference) {
        auto ref = dynamic_cast<const TypeReferenceNode*>(type_);
        return ref->dereference();
    }
    return type_;
}

void NamedValueNode::print(std::ostream &stream) const {
    stream << name_ << ": " << *type_;
}