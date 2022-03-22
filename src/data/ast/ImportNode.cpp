//
// Created by Michael Grossniklaus on 3/6/22.
//

#include "ImportNode.h"
#include "NodeVisitor.h"

//ImportNode::ImportNode(const FilePos &pos, std::unique_ptr<Identifier> alias, std::unique_ptr<Identifier> module) :
//        Node(NodeType::import, pos),
//        alias_(std::move(alias)), module_(std::move(module)) { }
//
//ImportNode::~ImportNode() = default;

Identifier* ImportNode::getAlias() const {
    return alias_.get();
}

Identifier* ImportNode::getModule() const {
    return module_.get();
}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ImportNode::print(std::ostream &stream) const {
    stream << (alias_ ? "" : to_string(*alias_) + " := ") << to_string(*module_);
}