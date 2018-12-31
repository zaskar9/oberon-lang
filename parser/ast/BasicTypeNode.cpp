/*
 * Implementation of the AST basic type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "BasicTypeNode.h"
#include "NodeVisitor.h"

BasicTypeNode::BasicTypeNode(const std::string &name, const int size) :
        TypeNode(NodeType::basic_type, { }, size), name_(name) {
}

BasicTypeNode::~BasicTypeNode() = default;

const std::string BasicTypeNode::getName() const {
    return name_;
}

void BasicTypeNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void BasicTypeNode::print(std::ostream &out) const {
    out << name_;
}

BasicTypeNode* BasicTypeNode::BOOLEAN = new BasicTypeNode("BOOLEAN", 1);
BasicTypeNode* BasicTypeNode::INTEGER = new BasicTypeNode("INTEGER", 4);
BasicTypeNode* BasicTypeNode::STRING = new BasicTypeNode("STRING", -1);
