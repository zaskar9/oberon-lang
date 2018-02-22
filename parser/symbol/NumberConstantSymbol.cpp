/*
 * Implementation of the class for the number constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#include "NumberConstantSymbol.h"

NumberConstantSymbol::NumberConstantSymbol(const std::string &name, int value) :
        ConstantSymbol(SymbolType::number_const, name), value_(value) {
}

NumberConstantSymbol::~NumberConstantSymbol() = default;

const int NumberConstantSymbol::getValue() const {
    return value_;
}

void NumberConstantSymbol::print(std::ostream &stream) const {
    stream << value_;
}
