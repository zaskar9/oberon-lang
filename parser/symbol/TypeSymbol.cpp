/*
 * Implementation of the base class of the type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "TypeSymbol.h"

TypeSymbol::TypeSymbol(const SymbolType type, const std::string &name, const int size) :
        Symbol(type, name), size_(size) {
}

TypeSymbol::~TypeSymbol() = default;

const int TypeSymbol::getSize() const {
    return size_;
}
