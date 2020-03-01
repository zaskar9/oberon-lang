/*
 * Number token returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#include "StringToken.h"

std::string StringToken::getValue() const {
    return value_;
}

void StringToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << value_;
}

