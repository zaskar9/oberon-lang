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

const std::shared_ptr<const BasicTypeNode> BasicTypeNode::INTEGER = std::shared_ptr<const BasicTypeNode>(new BasicTypeNode("INTEGER", 4));

//static const auto INTEGER = std::make_shared<const BasicTypeNode>("INTEGER", 4);
//static const auto BOOLEAN = std::make_shared<const BasicTypeNode>("BOOLEAN", 1);
//static const auto STRING  = std::make_shared<const BasicTypeNode>("STRING", -1);
