/*
 * String token returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#ifndef OBERON0C_STRINGTOKEN_H
#define OBERON0C_STRINGTOKEN_H


#include "Token.h"

class StringToken final : public Token {

private:
    std::string value_;

public:
    explicit StringToken(const FilePos &pos, std::string value) :
            Token(TokenType::const_string, pos), value_(std::move(value)) { };
    ~StringToken() override = default;

    [[nodiscard]] std::string getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_STRINGTOKEN_H
