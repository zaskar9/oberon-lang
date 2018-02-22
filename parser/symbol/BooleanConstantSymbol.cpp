/*
 * Implementation of the class for the Boolean constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#include "BooleanConstantSymbol.h"

BooleanConstantSymbol::BooleanConstantSymbol(const std::string &name, bool value) :
        ConstantSymbol(SymbolType::boolean_const, name), value_(value) {
}

BooleanConstantSymbol::~BooleanConstantSymbol() = default;

const bool BooleanConstantSymbol::getValue() const {
    return value_;
}

void BooleanConstantSymbol::print(std::ostream &stream) const {
    stream << (value_ ? "TRUE" : "FALSE");
}
