/*
 * AST node representing a basic type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "BasicTypeNode.h"
#include "NodeVisitor.h"


Ident *BasicTypeNode::getIdentifier() const {
    auto ident = TypeNode::getIdentifier();
    return ident ? ident : ident_.get();
}

void BasicTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void BasicTypeNode::print(std::ostream &out) const {
    out << *this->getIdentifier();
}