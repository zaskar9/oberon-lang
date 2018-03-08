/*
 * Implementation of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "SymbolTable.h"
#include "../ast/BasicTypeNode.h"

SymbolTable::SymbolTable(const SymbolTable *super) : super_(super), map_() {
}

SymbolTable::SymbolTable() : SymbolTable(nullptr) {
    // initialize global scope
    insert("INTEGER", std::make_unique<Symbol>(SymbolType::type, std::make_unique<BasicTypeNode>("INTEGER", 4)));
    insert("BOOLEAN", std::make_unique<Symbol>(SymbolType::type, std::make_unique<BasicTypeNode>("BOOLEAN", 1)));
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::insert(const std::string &name, std::unique_ptr<const Symbol> symbol) {
    map_[name] = std::move(symbol);
}

const Symbol* SymbolTable::lookup(const std::string &name) const {
    auto itr = map_.find(name);
    if (itr == map_.end()) {
        if (super_ != nullptr) {
            return super_->lookup(name);
        }
        return nullptr;
    }
    return (itr->second).get();
}

const bool SymbolTable::exists(const std::string &name) const {
    return map_[name] != nullptr;
}

std::unique_ptr<SymbolTable> SymbolTable::openScope() {
    return std::make_unique<SymbolTable>(this);
}