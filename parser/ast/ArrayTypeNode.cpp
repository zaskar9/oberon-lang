/*
 * Implementation of the AST array type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeNode.h"

ArrayTypeNode::ArrayTypeNode(const FilePos pos, const int dim, std::unique_ptr<const TypeNode> memberType) :
        TypeNode(NodeType::array_type, pos, dim * memberType->getSize()), dim_(dim), memberType_(std::move(memberType)) {
}

ArrayTypeNode::~ArrayTypeNode() = default;

const int ArrayTypeNode::getDimension() const {
    return dim_;
}

const TypeNode* ArrayTypeNode::getMemberType() const {
    return memberType_.get();
}

void ArrayTypeNode::print(std::ostream &out) const {
    out << "ARRAY " << dim_ << " OF " << *memberType_;
}
