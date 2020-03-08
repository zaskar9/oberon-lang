/*
 * Symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "SymbolTable.h"
#include "../ast/BasicTypeNode.h"

SymbolTable::SymbolTable() {
    level_ = 0;
    std::unique_ptr<Scope> predefined = std::make_unique<Scope>(nullptr);
    predefined->insert(BasicTypeNode::BOOLEAN->getName(), BasicTypeNode::BOOLEAN);
    predefined->insert(BasicTypeNode::INTEGER->getName(), BasicTypeNode::INTEGER);
    predefined->insert(BasicTypeNode::STRING->getName(), BasicTypeNode::STRING);
    scope_ = std::make_unique<Scope>(std::move(predefined));
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::insert(const std::string &name, Node *node) {
    scope_->insert(name, node);
}

Node* SymbolTable::lookup(const std::string &name) const {
    return scope_->lookup(name, false);
}

bool SymbolTable::isDefined(const std::string &name) const {
    return scope_->lookup(name, false) != nullptr;
}

bool SymbolTable::isDuplicate(const std::string &name) const {
    return scope_->lookup(name, true) != nullptr;
}

void SymbolTable::enterScope() {
    level_++;
    scope_ = std::make_unique<Scope>(std::move(scope_));
}

void SymbolTable::leaveScope() {
    if (level_ > 0) {
        scope_ = scope_->getParent();
        level_--;
    } else {
        // TODO throw exception
        std::cerr << "Illegal symtab table state: cannot leave current scope." << std::endl;
        exit(1);
    }
}

unsigned int SymbolTable::getLevel() const {
    return level_;
}