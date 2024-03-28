/*
 * Literal tokens returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/1/20.
 */

#ifndef OBERON_LANG_LITERALTOKEN_H
#define OBERON_LANG_LITERALTOKEN_H


#include "Token.h"

template <class T>
class LiteralToken : public Token {

private:
    T value_;

public:
    LiteralToken(const TokenType type, const FilePos &start, const FilePos &end, T value) :
            Token(type, start, end), value_(value) {};
    ~LiteralToken() override = default;

    [[nodiscard]] T value() const {
        return value_;
    }

    void print(std::ostream &stream) const override {
        stream << this->type() << ": " << value_;
    }

};


class BooleanLiteralToken : public LiteralToken<bool> {

public:
    BooleanLiteralToken(const FilePos &start, const FilePos &end, bool value) :
            LiteralToken<bool>(TokenType::boolean_literal, start, end, value) {};
    ~BooleanLiteralToken() noexcept override;

    void print(std::ostream &stream) const override;

};

class ShortLiteralToken final : public LiteralToken<int16_t> {

public:
    ShortLiteralToken(const FilePos &start, const FilePos &end, int16_t value) :
            LiteralToken<int16_t>(TokenType::short_literal, start, end, value) {};
    ~ShortLiteralToken() noexcept override;
};

class IntLiteralToken final : public LiteralToken<int32_t> {

public:
    IntLiteralToken(const FilePos &start, const FilePos &end, int32_t value) :
            LiteralToken<int32_t>(TokenType::int_literal, start, end, value) {};
    ~IntLiteralToken() noexcept override;

};


class LongLiteralToken final : public LiteralToken<int64_t> {

public:
    LongLiteralToken(const FilePos &start, const FilePos &end, int64_t value) :
            LiteralToken<int64_t>(TokenType::long_literal, start, end, value) {};
    ~LongLiteralToken() noexcept override;

};


class FloatLiteralToken final : public LiteralToken<float> {

public:
    FloatLiteralToken(const FilePos &start, const FilePos &end, float value) :
            LiteralToken<float>(TokenType::float_literal, start, end, value) {};
    ~FloatLiteralToken() noexcept override;

};


class DoubleLiteralToken final : public LiteralToken<double> {

public:
    DoubleLiteralToken(const FilePos &start, const FilePos &end, double value) :
            LiteralToken<double>(TokenType::double_literal, start, end, value) {};
    ~DoubleLiteralToken() noexcept override;

};


class StringLiteralToken final : public LiteralToken<std::string> {

public:
    StringLiteralToken(const FilePos &start, const FilePos &end, std::string value) :
            LiteralToken<std::string>(TokenType::string_literal, start, end, std::move(value)) {};
    ~StringLiteralToken() noexcept override;

};

class CharLiteralToken final : public LiteralToken<uint8_t> {

public:
    CharLiteralToken(const FilePos &start, const FilePos &end, uint8_t value) :
            LiteralToken<uint8_t>(TokenType::char_literal, start, end, value) {};
    ~CharLiteralToken() noexcept override;

};


#endif //OBERON_LANG_LITERALTOKEN_H
