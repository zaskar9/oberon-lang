/*
 * Number token returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#include "NumberToken.h"

int NumberToken::getValue() const {
    return value_;
}

void NumberToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << value_;
}