/*
 * Implementation of the base class of the basic type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "BasicTypeSymbol.h"

BasicTypeSymbol::BasicTypeSymbol(const std::string &name, const int size) :
        TypeSymbol(SymbolType::basic_type, name, size) {
}

BasicTypeSymbol::~BasicTypeSymbol() = default;

void BasicTypeSymbol::print(std::ostream &out) const {
    out << getName();
}