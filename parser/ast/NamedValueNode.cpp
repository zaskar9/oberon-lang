/*
 * Implementation of the AST named-value nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "NamedValueNode.h"
#include "NodeVisitor.h"

NamedValueNode::NamedValueNode(NodeType nodeType, const FilePos pos, const std::string &name, TypeNode* type, int level, int offset) :
        Node(nodeType, pos), name_(name), type_(type), level_(level), offset_(offset) {
}

NamedValueNode::~NamedValueNode() = default;

const std::string NamedValueNode::getName() const {
    return name_;
}

TypeNode* NamedValueNode::getType() const {
    return type_;
}

void NamedValueNode::print(std::ostream &stream) const {
    stream << name_ << ": " << *type_;
}

int NamedValueNode::getLevel() const {
    return level_;
}

int NamedValueNode::getOffset() const {
    return offset_;
}

void FieldNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void TypeDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void VariableNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}
