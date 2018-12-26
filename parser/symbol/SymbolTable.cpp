/*
 * Implementation of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "SymbolTable.h"
#include "../ast/BasicTypeNode.h"

SymbolTable::SymbolTable() {
    level_ = 0;
    std::unique_ptr<Scope> predefined = std::make_unique<Scope>(nullptr, -1);
    predefined->insert(BasicTypeNode::BOOLEAN->getName(), BasicTypeNode::BOOLEAN);
    predefined->insert(BasicTypeNode::INTEGER->getName(), BasicTypeNode::INTEGER);
    predefined->insert(BasicTypeNode::STRING->getName(), BasicTypeNode::STRING);
    scope_ = std::make_unique<Scope>(std::move(predefined), level_);
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::insert(const std::string &name, std::shared_ptr<Node> node) {
    scope_->insert(name, node);
}

std::shared_ptr<Node> SymbolTable::lookup(const std::string &name) const {
    return scope_->lookup(name, false);
}

const bool SymbolTable::isDefined(const std::string &name) const {
    return scope_->lookup(name, false) != nullptr;
}

const bool SymbolTable::isDuplicate(const std::string &name) const {
    return scope_->lookup(name, true) != nullptr;
}

void SymbolTable::enterScope() {
    scope_ = std::make_unique<Scope>(std::move(scope_), ++level_);
}

void SymbolTable::leaveScope() {
    if (level_ > 0) {
        scope_ = std::move(scope_->getParent());
        level_--;
    } else {
        std::cout << "Illegal leaveScope() call" << std::endl;
    }
}