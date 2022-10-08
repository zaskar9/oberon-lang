/*
 * AST node representing an array type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeNode.h"
#include "NodeVisitor.h"

ExpressionNode *ArrayTypeNode::getExpression() const {
    return expr_.get();
}

void ArrayTypeNode::setDimension(unsigned int dim) {
    dim_ = dim;
}

unsigned int ArrayTypeNode::getDimension() const {
    return dim_;
}

void ArrayTypeNode::setMemberType(TypeNode *memberType) {
    memberType_ = memberType;
}

TypeNode *ArrayTypeNode::getMemberType() const {
    return memberType_;
}

void ArrayTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ArrayTypeNode::print(std::ostream &out) const {
    if (this->isAnonymous()) {
        out << "ARRAY " << dim_ << " OF " << *memberType_;
    } else {
        out << *this->getIdentifier();
    }
}
