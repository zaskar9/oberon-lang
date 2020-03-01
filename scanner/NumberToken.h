/*
 * Number token returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#ifndef OBERON0C_NUMBERTOKEN_H
#define OBERON0C_NUMBERTOKEN_H


#include "Token.h"

class NumberToken final : public Token {

private:
    int value_;

public:
    explicit NumberToken(const FilePos &pos, int value) :
            Token(TokenType::const_number, pos), value_(value) { };
    ~NumberToken() override = default;

    [[nodiscard]] int getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_NUMBERTOKEN_H
