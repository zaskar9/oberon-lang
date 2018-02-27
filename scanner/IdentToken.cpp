/*
 * Implementation of the identifier tokens used by parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#include "IdentToken.h"

IdentToken::IdentToken(const FilePos pos, const std::string &value) :
        Token(TokenType::const_ident, pos), value_(value) {
}

IdentToken::~IdentToken() = default;

const std::string IdentToken::getValue() const {
    return value_;
}

void IdentToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << value_;
}