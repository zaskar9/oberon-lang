/*
 * Implementation of the AST type reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#include "TypeReferenceNode.h"

TypeReferenceNode::TypeReferenceNode(const FilePos pos, const TypeNode* type) :
        TypeNode(NodeType::type_reference, pos, type->getSize()), type_(type) {
}

TypeReferenceNode::~TypeReferenceNode() = default;

const TypeNode* TypeReferenceNode::dereference() const {
    return type_;
}

void TypeReferenceNode::print(std::ostream &stream) const {
    stream << *type_;
}
