/*
 * Implementation of the string tokens used by parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#include "StringToken.h"

StringToken::StringToken(const FilePos pos, const std::string &value) :
        Token(TokenType::const_string, pos), value_(value) {
}

StringToken::~StringToken() = default;

const std::string StringToken::getValue() const {
    return value_;
}

void StringToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << value_;
}

