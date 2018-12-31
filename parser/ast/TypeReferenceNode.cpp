/*
 * Implementation file of the type reference used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/26/18.
 */

#include "TypeReferenceNode.h"
#include "NodeVisitor.h"

TypeReferenceNode::TypeReferenceNode(const FilePos pos, const TypeNode *type) :
    TypeNode(NodeType::type_reference, pos, type->getSize()), type_(type) {
}

TypeReferenceNode::~TypeReferenceNode() = default;

const TypeNode* TypeReferenceNode::dereference() const {
    return type_;
}

void TypeReferenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void TypeReferenceNode::print(std::ostream &out) const {
    out << *type_;
}
