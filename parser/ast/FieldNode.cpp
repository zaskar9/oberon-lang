/*
 * Implementation of the AST field declaration nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "FieldNode.h"

FieldNode::FieldNode(const FilePos pos, const std::string &name, std::unique_ptr<const TypeNode> type) :
        Node(NodeType::field, pos), name_(name), type_(std::move(type)) {
}

FieldNode::~FieldNode() = default;

const std::string FieldNode::getName() const {
    return name_;
}

const TypeNode* FieldNode::getType() const {
    return type_.get();
}

void FieldNode::print(std::ostream &out) const {
    out << name_ << ": " << *type_;
}
