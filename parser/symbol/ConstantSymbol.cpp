/*
 * Implementation of the class for the constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#include "ConstantSymbol.h"

ConstantSymbol::ConstantSymbol(SymbolType type, const std::string &name) : Symbol(type, name) {
}

ConstantSymbol::~ConstantSymbol() = default;