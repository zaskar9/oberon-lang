/*
 * Implementation of the AST reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "ReferenceNode.h"
#include "NodeVisitor.h"

DeclarationNode* ReferenceNode::dereference() const {
    return node_;
}

void ReferenceNode::addSelector(std::unique_ptr<ExpressionNode> selector) {
    if (selector != nullptr) {
        selectors_.push_back(std::move(selector));
    }
}

ExpressionNode* ReferenceNode::getSelector(size_t num) const {
    return selectors_[num].get();
}

size_t ReferenceNode::getSelectorCount() const {
    return selectors_.size();
}


bool ReferenceNode::isConstant() const {
    return node_->getNodeType() == NodeType::constant;
}

void ReferenceNode::setType(class TypeNode *type) {
    type_ = type;
}

TypeNode* ReferenceNode::getType() const {
    if (type_ == nullptr) {
        return node_->getType();
    }
    return type_;
}

void ReferenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ReferenceNode::print(std::ostream &stream) const {
    stream << *node_;
}


