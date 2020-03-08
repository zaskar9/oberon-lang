/*
 * AST node representing a basic type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "BasicTypeNode.h"
#include "NodeVisitor.h"

void BasicTypeNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void BasicTypeNode::print(std::ostream &out) const {
    out << this->getName();
}

BasicTypeNode* BasicTypeNode::BOOLEAN = new BasicTypeNode("BOOLEAN", 1);
BasicTypeNode* BasicTypeNode::CHAR = new BasicTypeNode("CHAR", 1);
BasicTypeNode* BasicTypeNode::BYTE = new BasicTypeNode("BYTE", 1);
BasicTypeNode* BasicTypeNode::INTEGER = new BasicTypeNode("INTEGER", 4);
BasicTypeNode* BasicTypeNode::LONGINT = new BasicTypeNode("LONGINT", 8);
BasicTypeNode* BasicTypeNode::REAL = new BasicTypeNode("REAL", 4);
BasicTypeNode* BasicTypeNode::LONGREAL = new BasicTypeNode("LONGREAL", 8);
BasicTypeNode* BasicTypeNode::STRING = new BasicTypeNode("STRING", 0);
BasicTypeNode* BasicTypeNode::UNDEF = new BasicTypeNode("UNDEFINED", 0);
