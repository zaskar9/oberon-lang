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
            Token(type, start, end), value_(value) { };
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
            LiteralToken(TokenType::boolean_literal, start, end, value) { };
    ~BooleanLiteralToken() noexcept override;

    void print(std::ostream &stream) const override;

};


class IntegerLiteralToken final : public LiteralToken<int> {

public:
    explicit IntegerLiteralToken(const FilePos &start, const FilePos &end, int value) :
            LiteralToken(TokenType::integer_literal, start, end, value) { };
    ~IntegerLiteralToken() noexcept override;

};


class LongintLiteralToken final : public LiteralToken<long> {

public:
    explicit LongintLiteralToken(const FilePos &start, const FilePos &end, long value) :
            LiteralToken(TokenType::longint_literal, start, end, value) { };
    ~LongintLiteralToken() noexcept override;

};


class RealLiteralToken final : public LiteralToken<float> {

public:
    explicit RealLiteralToken(const FilePos &start, const FilePos &end, float value) :
            LiteralToken(TokenType::real_literal, start, end, value) { };
    ~RealLiteralToken() noexcept override;

};


class LongrealLiteralToken final : public LiteralToken<double> {

public:
    explicit LongrealLiteralToken(const FilePos &start, const FilePos &end, double value) :
            LiteralToken(TokenType::longreal_literal, start, end, value) { };
    ~LongrealLiteralToken() noexcept override;

};

class StringLiteralToken final : public LiteralToken<std::string> {

public:
    explicit StringLiteralToken(const FilePos &start, const FilePos &end, std::string value) :
            LiteralToken(TokenType::string_literal, start, end, std::move(value)) { };
    ~StringLiteralToken() noexcept override;

};


#endif //OBERON_LANG_LITERALTOKEN_H
