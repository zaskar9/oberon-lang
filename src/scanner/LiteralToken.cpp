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

IntegerLiteralToken::~IntegerLiteralToken() noexcept = default;

LongintLiteralToken::~LongintLiteralToken() noexcept = default;

RealLiteralToken::~RealLiteralToken() noexcept = default;

LongrealLiteralToken::~LongrealLiteralToken() noexcept = default;

StringLiteralToken::~StringLiteralToken() noexcept = default;
