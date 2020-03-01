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
BasicTypeNode* BasicTypeNode::BYTE = new BasicTypeNode("INTEGER", 1);
BasicTypeNode* BasicTypeNode::SHORTINT = new BasicTypeNode("INTEGER", 2);
BasicTypeNode* BasicTypeNode::INTEGER = new BasicTypeNode("INTEGER", 4);
BasicTypeNode* BasicTypeNode::LONGINT = new BasicTypeNode("INTEGER", 8);
BasicTypeNode* BasicTypeNode::STRING = new BasicTypeNode("STRING", -1);
