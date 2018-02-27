/*
 * Implementation of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "SymbolTable.h"
#include "ast/BasicTypeNode.h"

SymbolTable::SymbolTable(std::unique_ptr<SymbolTable> super, Logger *logger) :
        id_(0), logger_(logger), super_(std::move(super)), map_() {
}

SymbolTable::SymbolTable(Logger *logger) : SymbolTable(nullptr, logger) {
    // initialize global scope
    insert("INTEGER", std::make_unique<BasicTypeNode>("INTEGER", 4));
    insert("BOOLEAN", std::make_unique<BasicTypeNode>("BOOLEAN", 1));
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::insert(const std::string &name, std::unique_ptr<const Node> symbol) {
    map_[name] = std::move(symbol);
}

const Node* SymbolTable::lookup(const std::string &name) const {
    auto itr = map_.find(name);
    return itr != map_.end() ? (itr->second).get() : nullptr;
}

const bool SymbolTable::isGlobal() const {
    return super_ == nullptr;
}