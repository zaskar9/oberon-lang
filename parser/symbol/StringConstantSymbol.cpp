/*
 * Implementation of the class for the Boolean constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#include "StringConstantSymbol.h"

StringConstantSymbol::StringConstantSymbol(const std::string &name, const std::string &value) :
        ConstantSymbol(SymbolType::string_const, name), value_(value) {
}

StringConstantSymbol::~StringConstantSymbol() = default;

const std::string StringConstantSymbol::getValue() const {
    return value_;
}

void StringConstantSymbol::print(std::ostream &stream) const {
    stream << value_;
}
