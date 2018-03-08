/*
 * Implementation of the base class of all symbols used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "Symbol.h"

Symbol::Symbol(const SymbolType type, const Node *node) : type_(type), node_(node) {
}

Symbol::~Symbol() = default;

const SymbolType Symbol::getType() const {
    return type_;
}

const Node* Symbol::getNode() const {
    return node_;
}

void Symbol::print(std::ostream &stream) const {
    stream << *node_;
}

std::ostream& operator<<(std::ostream &stream, const Symbol &symbol) {
    symbol.print(stream);
    return stream;
}