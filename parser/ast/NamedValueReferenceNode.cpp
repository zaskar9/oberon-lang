/*
 * Implementation of the AST variable reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "NamedValueReferenceNode.h"

NamedValueReferenceNode::NamedValueReferenceNode(FilePos pos, const NamedValueNode *node) :
        ExpressionNode(NodeType::name_reference, pos), node_(node) {
}

NamedValueReferenceNode::~NamedValueReferenceNode() = default;

const NamedValueNode* NamedValueReferenceNode::dereference() const {
    return node_;
}

bool NamedValueReferenceNode::isConstant() const {
    return node_->getNodeType() == NodeType::constant;
}

const TypeNode* NamedValueReferenceNode::getType() const {
    return node_->getType();
}

void NamedValueReferenceNode::print(std::ostream &stream) const {
    stream << *node_;
}


