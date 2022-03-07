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
    out << *this->getIdentifier();
}

BasicTypeNode* BasicTypeNode::BOOLEAN = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "BOOLEAN"), 1);
BasicTypeNode* BasicTypeNode::CHAR = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "CHAR"), 1);
BasicTypeNode* BasicTypeNode::BYTE = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "BYTE"), 1);
BasicTypeNode* BasicTypeNode::INTEGER = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "INTEGER"), 4);
BasicTypeNode* BasicTypeNode::LONGINT = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "LONGINT"), 8);
BasicTypeNode* BasicTypeNode::REAL = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "REAL"), 4);
BasicTypeNode* BasicTypeNode::LONGREAL = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "LONGREAL"), 8);
BasicTypeNode* BasicTypeNode::STRING = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "STRING"), 0);
BasicTypeNode* BasicTypeNode::UNDEF = new BasicTypeNode(std::make_unique<Identifier>(EMPTY_POS, "UNDEFINED"), 0);
