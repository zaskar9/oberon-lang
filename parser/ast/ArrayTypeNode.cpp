/*
 * Implementation of the AST array type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeNode.h"

ArrayTypeNode::ArrayTypeNode(const FilePos pos, const int dim, const std::shared_ptr<const TypeNode> &memberType) :
        TypeNode(NodeType::array_type, pos, dim * memberType->getSize()), dim_(dim), memberType_(memberType) {
}

ArrayTypeNode::~ArrayTypeNode() = default;

const int ArrayTypeNode::getDimension() const {
    return dim_;
}

const std::shared_ptr<const TypeNode> ArrayTypeNode::getMemberType() const {
    return memberType_;
}

void ArrayTypeNode::print(std::ostream &out) const {
    out << "ARRAY " << dim_ << " OF " << *memberType_;
}
