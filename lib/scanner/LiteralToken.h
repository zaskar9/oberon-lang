/*
 * Literal tokens returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/1/20.
 */

#ifndef OBERON_LANG_LITERALTOKEN_H
#define OBERON_LANG_LITERALTOKEN_H


#include <cstdint>
#include <string>
#include <utility>
#include "Token.h"


using std::string;

template <class T>
class LiteralToken : public Token {

public:
    LiteralToken(const TokenType type, const FilePos &start, const FilePos &end, T value) :
            Token(type, start, end), value_(std::move(value)) {}
    ~LiteralToken() override = default;

    [[nodiscard]] T value() const {
        return value_;
    }

    void print(std::ostream &stream) const override {
        stream << this->type() << ": " << value_;
    }

private:
    T value_;

};


class BooleanLiteralToken final : public LiteralToken<bool> {

public:
    BooleanLiteralToken(const FilePos &start, const FilePos &end, const bool value) :
            LiteralToken(TokenType::boolean_literal, start, end, value) {}
    ~BooleanLiteralToken() noexcept override;

    void print(std::ostream &stream) const override;

};

class ShortLiteralToken final : public LiteralToken<int16_t> {

public:
    ShortLiteralToken(const FilePos &start, const FilePos &end, const int16_t value) :
            LiteralToken(TokenType::short_literal, start, end, value) {}
    ~ShortLiteralToken() noexcept override;
};

class IntLiteralToken final : public LiteralToken<int32_t> {

public:
    IntLiteralToken(const FilePos &start, const FilePos &end, const int32_t value) :
            LiteralToken(TokenType::int_literal, start, end, value) {}
    ~IntLiteralToken() noexcept override;

};


class LongLiteralToken final : public LiteralToken<int64_t> {

public:
    LongLiteralToken(const FilePos &start, const FilePos &end, const int64_t value) :
            LiteralToken(TokenType::long_literal, start, end, value) {}
    ~LongLiteralToken() noexcept override;

};


class FloatLiteralToken final : public LiteralToken<float> {

public:
    FloatLiteralToken(const FilePos &start, const FilePos &end, const float value) :
            LiteralToken(TokenType::float_literal, start, end, value) {}
    ~FloatLiteralToken() noexcept override;

};


class DoubleLiteralToken final : public LiteralToken<double> {

public:
    DoubleLiteralToken(const FilePos &start, const FilePos &end, const double value) :
            LiteralToken(TokenType::double_literal, start, end, value) {}
    ~DoubleLiteralToken() noexcept override;

};


class StringLiteralToken final : public LiteralToken<string> {

public:
    StringLiteralToken(const FilePos &start, const FilePos &end, string value) :
            LiteralToken(TokenType::string_literal, start, end, std::move(value)) {}
    ~StringLiteralToken() noexcept override;

};

class CharLiteralToken final : public LiteralToken<uint8_t> {

public:
    CharLiteralToken(const FilePos &start, const FilePos &end, const uint8_t value) :
            LiteralToken(TokenType::char_literal, start, end, value) {}
    ~CharLiteralToken() noexcept override;

};


#endif //OBERON_LANG_LITERALTOKEN_H
