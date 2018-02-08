/*
 * Implementation of the base class of all symbols used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "Symbol.h"

Symbol::Symbol(const SymbolType type, const std::string &name) : type_(type), name_(name) {

}

Symbol::~Symbol() = default;

const SymbolType Symbol::getType() const {
    return type_;
}

const std::string Symbol::getName() const {
    return name_;
}