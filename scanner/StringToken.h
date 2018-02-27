/*
 * Header file of the string tokens used by parser of the Oberon-0 compiler.
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
    explicit StringToken(FilePos pos, const std::string &value);
    ~StringToken() override;

    const std::string getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_STRINGTOKEN_H
