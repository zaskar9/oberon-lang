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
    explicit LiteralToken(const TokenType type, const FilePos &pos, T value) :
            Token(type, pos), value_(value) { };
    ~LiteralToken() override = default;

    [[nodiscard]] T value() const {
        return value_;
    };

    void print(std::ostream &stream) const override {
        stream << this->getType() << ": " << value_;
    };

};


class BooleanLiteralToken : public LiteralToken<bool> {

public:
    explicit BooleanLiteralToken(const FilePos &pos, bool value) :
            LiteralToken(TokenType::boolean_literal, pos, value) { };
    ~BooleanLiteralToken() override = default;

    void print(std::ostream &stream) const override;

};


class IntegerLiteralToken final : public LiteralToken<int> {

public:
    explicit IntegerLiteralToken(const FilePos &pos, int value) :
            LiteralToken(TokenType::integer_literal, pos, value) { };
    ~IntegerLiteralToken() override = default;

};


class LongintLiteralToken final : public LiteralToken<long> {

public:
    explicit LongintLiteralToken(const FilePos &pos, long value) :
            LiteralToken(TokenType::longint_literal, pos, value) { };
    ~LongintLiteralToken() override = default;

};


class RealLiteralToken final : public LiteralToken<float> {

public:
    explicit RealLiteralToken(const FilePos &pos, float value) :
            LiteralToken(TokenType::real_literal, pos, value) { };
    ~RealLiteralToken() override = default;

};


class LongrealLiteralToken final : public LiteralToken<double> {

public:
    explicit LongrealLiteralToken(const FilePos &pos, double value) :
            LiteralToken(TokenType::longreal_literal, pos, value) { };
    ~LongrealLiteralToken() override = default;

};

class StringLiteralToken final : public LiteralToken<std::string> {

public:
    explicit StringLiteralToken(const FilePos &pos, std::string value) :
            LiteralToken(TokenType::string_literal, pos, std::move(value)) { };
    ~StringLiteralToken() override = default;

};


#endif //OBERON_LANG_LITERALTOKEN_H
