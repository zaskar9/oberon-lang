/*
 * AST node representing an array type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeNode.h"
#include "NodeVisitor.h"

void ArrayTypeNode::setDimension(unsigned int dim) {
    dimension_ = dim;
}

unsigned int ArrayTypeNode::getDimension() const {
    return dimension_;
}

void ArrayTypeNode::setMemberType(TypeNode *memberType) {
    memberType_ = memberType;
}

TypeNode *ArrayTypeNode::getMemberType() const {
    return memberType_;
}

bool ArrayTypeNode::isOpen() const {
    return dimension_ == 0;
}

void ArrayTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ArrayTypeNode::print(std::ostream &out) const {
    if (this->isAnonymous()) {
        out << "array type";
    } else {
        out << *this->getIdentifier();
    }
}
