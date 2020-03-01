/*
 * Implementation of the AST array type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeNode.h"
#include "NodeVisitor.h"

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
