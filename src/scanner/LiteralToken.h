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
    explicit LiteralToken(const TokenType type, const FilePos &start, const FilePos &end, T value) :
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
    explicit BooleanLiteralToken(const FilePos &start, const FilePos &end, bool value) :
            LiteralToken(TokenType::boolean_literal, start, end, value) {};
    ~BooleanLiteralToken() noexcept override;

    void print(std::ostream &stream) const override;

};


class IntLiteralToken final : public LiteralToken<int> {

public:
    explicit IntLiteralToken(const FilePos &start, const FilePos &end, int value) :
            LiteralToken(TokenType::int_literal, start, end, value) {};
    ~IntLiteralToken() noexcept override;

};


class LongLiteralToken final : public LiteralToken<long> {

public:
    explicit LongLiteralToken(const FilePos &start, const FilePos &end, long value) :
            LiteralToken(TokenType::long_literal, start, end, value) {};
    ~LongLiteralToken() noexcept override;

};


class FloatLiteralToken final : public LiteralToken<float> {

public:
    explicit FloatLiteralToken(const FilePos &start, const FilePos &end, float value) :
            LiteralToken(TokenType::float_literal, start, end, value) {};
    ~FloatLiteralToken() noexcept override;

};


class DoubleLiteralToken final : public LiteralToken<double> {

public:
    explicit DoubleLiteralToken(const FilePos &start, const FilePos &end, double value) :
            LiteralToken(TokenType::double_literal, start, end, value) {};
    ~DoubleLiteralToken() noexcept override;

};


class StringLiteralToken final : public LiteralToken<std::string> {

public:
    explicit StringLiteralToken(const FilePos &start, const FilePos &end, std::string value) :
            LiteralToken(TokenType::string_literal, start, end, std::move(value)) {};
    ~StringLiteralToken() noexcept override;

};

class CharLiteralToken final : public LiteralToken<unsigned char> {

public:
    explicit CharLiteralToken(const FilePos &start, const FilePos &end, unsigned char value) :
            LiteralToken(TokenType::char_literal, start, end, value) {};
    ~CharLiteralToken() noexcept override;

};


#endif //OBERON_LANG_LITERALTOKEN_H
