/*
 * Implementation of the undefined token used by scanner of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/28/18.
 */

#include "UndefinedToken.h"

UndefinedToken::UndefinedToken(const FilePos pos, const char value) : Token(TokenType::undef, pos), value_(value) {
}

UndefinedToken::~UndefinedToken() = default;

char UndefinedToken::getValue() {
    return value_;
}

void UndefinedToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << value_;
}