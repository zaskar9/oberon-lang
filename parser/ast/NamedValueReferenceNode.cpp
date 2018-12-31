/*
 * Implementation of the AST variable reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "NamedValueReferenceNode.h"
#include "NodeVisitor.h"

NamedValueReferenceNode::NamedValueReferenceNode(const FilePos pos, const NamedValueNode *node,
        std::unique_ptr<ExpressionNode> selector) : ExpressionNode(NodeType::name_reference, pos),
        node_(node), selector_(std::move(selector)) {
}

NamedValueReferenceNode::NamedValueReferenceNode(const FilePos pos, const NamedValueNode* node) :
        NamedValueReferenceNode(pos, node, nullptr) {
}

NamedValueReferenceNode::~NamedValueReferenceNode() = default;

const NamedValueNode* NamedValueReferenceNode::dereference() const {
    return node_;
}

const ExpressionNode* NamedValueReferenceNode::getSelector() const {
    return selector_.get();
}

bool NamedValueReferenceNode::isConstant() const {
    return node_->getNodeType() == NodeType::constant;
}

const TypeNode* NamedValueReferenceNode::getType() const {
    auto type = node_->getType();
    if (selector_ != nullptr) {
        if (type->getNodeType() == NodeType::record_type || type->getNodeType() == NodeType::array_type) {
            return selector_->getType();
        }
    }
    return type;
}

void NamedValueReferenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void NamedValueReferenceNode::print(std::ostream &stream) const {
    stream << *node_;
}


