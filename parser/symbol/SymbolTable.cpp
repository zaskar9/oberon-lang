/*
 * Implementation of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "SymbolTable.h"
#include "../ast/BasicTypeNode.h"

SymbolTable::SymbolTable(const SymbolTable *super) : super_(super), map_(), types_() {
}

SymbolTable::SymbolTable() : SymbolTable(nullptr) {
    // initialize global scope
    insertType(BasicTypeNode::INTEGER->getName(), BasicTypeNode::INTEGER);
    insertType(BasicTypeNode::BOOLEAN->getName(), BasicTypeNode::BOOLEAN);
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::insert(const std::string &name, const Node* node) {
    map_[name] = node;
}

void SymbolTable::insertType(const std::string &name, const std::shared_ptr<const TypeNode> &type) {
    types_[name] = type;
}

const Node* SymbolTable::lookup(const std::string &name) const {
    auto node = lookupLocal(map_, name);
    if (node == nullptr && super_ != nullptr) {
        return super_->lookup(name);
    }
    return node;
}

const std::shared_ptr<const TypeNode> SymbolTable::lookupType(const std::string &name) const {
    auto type = lookupLocal(types_, name);
    if (type == nullptr && super_ != nullptr) {
        return super_->lookupType(name);
    }
    return type;
}
const bool SymbolTable::exists(const std::string &name) const {
    return lookupLocal(map_, name) != nullptr;
}

const bool SymbolTable::existsType(const std::string &name) const {
    return lookupLocal(types_, name) != nullptr;
}

std::unique_ptr<SymbolTable> SymbolTable::openScope() {
    return std::unique_ptr<SymbolTable>(new SymbolTable(this));
}