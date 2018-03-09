/*
 * Implementation of the AST variable reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "ReferenceNode.h"

ReferenceNode::ReferenceNode(FilePos pos, const NamedValueNode* node) :
        ExpressionNode(NodeType::variable_reference, pos), node_(node) {
}

ReferenceNode::~ReferenceNode() = default;

const NamedValueNode* ReferenceNode::dereference() const {
    return node_;
}

bool ReferenceNode::isConstant() const {
    return false;
}

std::shared_ptr<const TypeNode> ReferenceNode::getType() const {
    return node_->getType();
}

void ReferenceNode::print(std::ostream &stream) const {
    stream << *node_;
}


