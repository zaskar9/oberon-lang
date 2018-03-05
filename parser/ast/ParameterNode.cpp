/*
 * Implementation of the AST parameter declaration nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ParameterNode.h"

ParameterNode::ParameterNode(FilePos pos, const std::string &name, std::unique_ptr<const TypeNode> type, bool var) :
        Node(NodeType::parameter, pos), name_(name), type_(std::move(type)), var_(var) {
}

ParameterNode::~ParameterNode() = default;

const std::string ParameterNode::getName() const {
    return name_;
}

const TypeNode* ParameterNode::getType() const {
    return type_.get();
}

const bool ParameterNode::isVar() const {
    return var_;
}

void ParameterNode::print(std::ostream &stream) const {
    stream << (var_ ? "VAR " : "") << name_ << ": " << *type_;
}