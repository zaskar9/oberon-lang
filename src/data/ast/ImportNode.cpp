//
// Created by Michael Grossniklaus on 3/6/22.
//

#include "ImportNode.h"
#include "ModuleNode.h"
#include "NodeVisitor.h"

ImportNode::ImportNode(const FilePos &pos, std::unique_ptr<Identifier> alias, std::unique_ptr<Identifier> name) :
        Node(NodeType::import, pos),
        alias_(std::move(alias)), module_(std::make_unique<ModuleNode>(pos, std::move(name))) { }

ImportNode::~ImportNode() = default;

Identifier* ImportNode::getAlias() const {
    return alias_.get();
}

ModuleNode* ImportNode::getModule() const {
    return module_.get();
}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ImportNode::print(std::ostream &stream) const {
    stream << (alias_ ? "" : to_string(*alias_) + " := ") << *module_->getIdentifier();
}