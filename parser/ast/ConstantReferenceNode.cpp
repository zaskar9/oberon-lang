/*
 * Implementation of the AST constant reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/28/18.
 */

#include "ConstantReferenceNode.h"

ConstantReferenceNode::ConstantReferenceNode(const FilePos pos, const ConstantNode *constant) :
        ConstantNode(NodeType::constant_reference, pos, constant->checkType()), constant_(constant) {
}

ConstantReferenceNode::~ConstantReferenceNode() = default;

const ConstantNode* ConstantReferenceNode::dereference() const {
    return constant_;
}

void ConstantReferenceNode::print(std::ostream &stream) const {
    stream << *constant_;
}