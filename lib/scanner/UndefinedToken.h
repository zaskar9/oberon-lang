/*
 * Undefined token returned scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/28/18.
 */

#ifndef OBERON_LANG_UNDEFINEDTOKEN_H
#define OBERON_LANG_UNDEFINEDTOKEN_H


#include "Token.h"

class UndefinedToken final : public Token {

public:
    explicit UndefinedToken(const FilePos &pos, char value) :
            Token(TokenType::undef, pos), value_(value) { };
    ~UndefinedToken() override = default;

    [[nodiscard]] char value();

    void print(std::ostream &stream) const override;

private:
    const char value_;

};


#endif //OBERON_LANG_UNDEFINEDTOKEN_H
