/*
 * Implementation file of the type reference used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/26/18.
 */

#include "TypeReferenceNode.h"


TypeReferenceNode::TypeReferenceNode(FilePos pos, const TypeNode *type) :
    TypeNode(NodeType::type_reference, pos, type->getSize()), type_(type) {
}

const TypeNode* TypeReferenceNode::dereference() const {
    return type_;
}

void TypeReferenceNode::print(std::ostream &out) const {
    out << *type_;
}
