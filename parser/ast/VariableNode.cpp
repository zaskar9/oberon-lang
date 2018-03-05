/*
 * Implementation of the AST variable declaration nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/2/18.
 */

#include "VariableNode.h"

VariableNode::VariableNode(const FilePos pos, const std::string &name, std::unique_ptr<const TypeNode> type) :
        ExpressionNode(NodeType::variable, pos), name_(name), type_(std::move(type)) {
}

VariableNode::~VariableNode() = default;

const std::string VariableNode::getName() const {
    return name_;
}

const TypeNode* VariableNode::getType() const {
    return type_.get();
}

bool VariableNode::isConstant() const {
    return false;
}

ExpressionType VariableNode::checkType() const {
    return ExpressionType::UNDEF;
}

void VariableNode::print(std::ostream &out) const {
    out << name_ << ": " << *type_;
}
