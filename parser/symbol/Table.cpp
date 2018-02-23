/*
 * Implementation of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "Table.h"
#include "BasicTypeSymbol.h"

Table::Table(std::unique_ptr<Table> super, Logger *logger) : map_(), super_(std::move(super)), logger_(logger) {

}

Table::Table(Logger *logger) : Table(nullptr, logger) {
    // initialize global scope
    insert(std::make_unique<BasicTypeSymbol>("INTEGER", 4));
    insert(std::make_unique<BasicTypeSymbol>("BOOLEAN", 1));
}

Table::~Table() = default;

void Table::insert(std::unique_ptr<const Symbol> symbol) {
    insert(symbol->getName(), std::move(symbol));
}

void Table::insert(const std::string &name, std::unique_ptr<const Symbol> symbol) {
    map_[name] = std::move(symbol);
}

const Symbol* Table::lookup(const std::string &name) const {
    auto itr = map_.find(name);
    return itr != map_.end() ? (itr->second).get() : nullptr;
}

const bool Table::isGlobal() const {
    return super_ == nullptr;
}