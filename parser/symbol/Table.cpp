/*
 * Implementation of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "Table.h"
#include "BasicTypeSymbol.h"

Table::Table(Table *super, Logger *log) : map_(), super_(super), log_(log) {

}

Table::Table(Logger *log) : Table(nullptr, log) {
    // initialize global scope
    insert(std::make_shared<BasicTypeSymbol>("INTEGER", 4));
    insert(std::make_shared<BasicTypeSymbol>("BOOLEAN", 1));
}

Table::~Table() = default;

void Table::insert(std::shared_ptr<const Symbol> symbol) {
    insert(symbol->getName(), symbol);
}

void Table::insert(const std::string &name, std::shared_ptr<const Symbol> symbol) {
    map_[name] = symbol;
}

std::shared_ptr<const Symbol> Table::lookup(const std::string &name) const {
    auto itr = map_.find(name);
    return itr != map_.end() ? itr->second : nullptr;
}

const bool Table::isGlobal() const {
    return super_ == nullptr;
}