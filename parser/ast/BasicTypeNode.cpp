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

const std::string BasicTypeNode::getName() const {
    return name_;
}

void BasicTypeNode::print(std::ostream &out) const {
    out << name_;
}

const std::shared_ptr<BasicTypeNode> BasicTypeNode::BOOLEAN = std::make_shared<BasicTypeNode>("BOOLEAN", 1);
const std::shared_ptr<BasicTypeNode> BasicTypeNode::INTEGER = std::make_shared<BasicTypeNode>("INTEGER", 4);
const std::shared_ptr<BasicTypeNode> BasicTypeNode::STRING = std::make_shared<BasicTypeNode>("STRING", -1);
