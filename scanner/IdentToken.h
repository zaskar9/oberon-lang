/*
 * Identifier token returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#ifndef OBERON0C_IDENTTOKEN_H
#define OBERON0C_IDENTTOKEN_H


#include "Token.h"

class IdentToken final : public Token {

private:
    std::string value_;

public:
    explicit IdentToken(const FilePos &pos, std::string value) :
            Token(TokenType::const_ident, pos), value_(std::move(value)) { };
    ~IdentToken() override = default;

    [[nodiscard]] std::string getValue() const;

    void print(std::ostream &stream) const override;
};


#endif //OBERON0C_IDENTTOKEN_H
