/*
 * Implementation of the AST array type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeNode.h"
#include "NodeVisitor.h"

ArrayTypeNode::ArrayTypeNode(const FilePos pos, const int dim, TypeNode* memberType) :
        TypeNode(NodeType::array_type, pos, dim * memberType->getSize()), dim_(dim), memberType_(memberType) {
}

ArrayTypeNode::~ArrayTypeNode() = default;

int ArrayTypeNode::getDimension() const {
    return dim_;
}

TypeNode* ArrayTypeNode::getMemberType() const {
    return memberType_;
}

void ArrayTypeNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ArrayTypeNode::print(std::ostream &out) const {
    out << "ARRAY " << dim_ << " OF " << *memberType_;
}
