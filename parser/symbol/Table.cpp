/*
 * Implementation of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "Table.h"

Table::Table(Table *super, Logger *log) : map_(), super_(super), log_(log) {

}

Table::Table(Logger *log) : Table(nullptr, log) {

}

Table::~Table() {
    // Delete all symbols in the table
    for (auto &itr : map_) {
        delete itr.second;
    }
}

void Table::insert(const Symbol *symbol) {
    map_[symbol->getName()] = symbol;
}

const Symbol* Table::lookup(const std::string &name) const {
    std::unordered_map<std::string, const Symbol*>::const_iterator itr = map_.find(name);
    if (itr != map_.end()) {
        return itr->second;
    }
    return NULL;
}