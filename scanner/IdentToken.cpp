/*
 * Identifier token returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#include "IdentToken.h"

std::string IdentToken::getValue() const {
    return value_;
}

void IdentToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << value_;
}