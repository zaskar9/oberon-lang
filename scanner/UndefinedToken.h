/*
 * Undefined token returned scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/28/18.
 */

#ifndef OBERON0C_UNDEFINEDTOKEN_H
#define OBERON0C_UNDEFINEDTOKEN_H


#include "Token.h"

class UndefinedToken : public Token {

private:
    const char value_;

public:
    explicit UndefinedToken(const FilePos &pos, char value) :
            Token(TokenType::undef, pos), value_(value) { };
    ~UndefinedToken() override = default;

    [[nodiscard]] char value();

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_UNDEFINEDTOKEN_H
