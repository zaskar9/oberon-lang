/*
 * Literal tokens returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/1/20.
 */

#include "LiteralToken.h"

BooleanLiteralToken::~BooleanLiteralToken() noexcept = default;

void BooleanLiteralToken::print(std::ostream &stream) const {
    stream << this->type() << ": " << (value() ? "TRUE" : "FALSE");
}

IntLiteralToken::~IntLiteralToken() noexcept = default;

LongLiteralToken::~LongLiteralToken() noexcept = default;

FloatLiteralToken::~FloatLiteralToken() noexcept = default;

DoubleLiteralToken::~DoubleLiteralToken() noexcept = default;

StringLiteralToken::~StringLiteralToken() noexcept = default;
