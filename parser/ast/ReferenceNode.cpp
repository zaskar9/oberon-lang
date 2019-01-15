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

ExpressionNode* ReferenceNode::getSelector() const {
    return selector_.get();
}

bool ReferenceNode::isConstant() const {
    return node_->getNodeType() == NodeType::constant;
}

TypeNode* ReferenceNode::getType() const {
    auto type = node_->getType();
    if (selector_ != nullptr) {
        if (type->getNodeType() == NodeType::record_type || type->getNodeType() == NodeType::array_type) {
            return selector_->getType();
        }
    }
    return type;
}

void ReferenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ReferenceNode::print(std::ostream &stream) const {
    stream << *node_;
}


