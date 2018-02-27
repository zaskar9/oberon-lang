/*
 * Implementation of the AST basic type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "BasicTypeNode.h"

BasicTypeNode::BasicTypeNode(const std::string &name, const int size) :
        TypeNode(NodeType::basic_type, { }, size), name_(name) {
}

BasicTypeNode::~BasicTypeNode() = default;

void BasicTypeNode::print(std::ostream &out) const {
    out << name_;
}