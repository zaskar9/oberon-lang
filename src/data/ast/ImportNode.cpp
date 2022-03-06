//
// Created by Michael Grossniklaus on 3/6/22.
//

#include "ImportNode.h"
#include "ModuleNode.h"
#include "NodeVisitor.h"

ImportNode::ImportNode(const FilePos &pos, std::string alias, std::string name) : Node(NodeType::import, pos),
        alias_(std::move(alias)), module_(std::make_unique<ModuleNode>(pos, name)) { }

ImportNode::~ImportNode() = default;

std::string ImportNode::getAlias() const {
    return alias_;
}

ModuleNode* ImportNode::getModule() const {
    return module_.get();
}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ImportNode::print(std::ostream &stream) const {
    stream << (alias_.empty() ? "" : alias_ + " := ") << module_->getName();
}