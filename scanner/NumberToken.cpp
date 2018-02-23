/*
 * Implementation of the class for the number token used by parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#include "NumberToken.h"

NumberToken::NumberToken(FilePos pos, int value) : Token(TokenType::const_number, pos), value_(value) {
}

NumberToken::~NumberToken() = default;

const int NumberToken::getValue() const {
    return value_;
}

void NumberToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << value_;
}