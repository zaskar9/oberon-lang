/*
 * Ident token returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#ifndef OBERON_LANG_IDENTTOKEN_H
#define OBERON_LANG_IDENTTOKEN_H


#include "Token.h"

class IdentToken final : public Token {

public:
    explicit IdentToken(const FilePos &start, const FilePos &end, std::string value) :
            Token(TokenType::const_ident, start, end), value_(std::move(value)) { };
    ~IdentToken() override = default;

    [[nodiscard]] std::string value() const;

    void print(std::ostream &stream) const override;

private:
    std::string value_;

};


#endif //OBERON_LANG_IDENTTOKEN_H
