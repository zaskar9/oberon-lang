/*
 * Header of the undefined token used by scanner of the Oberon-0 compiler.
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
    explicit UndefinedToken(FilePos pos, char value);
    ~UndefinedToken() override;

    char getValue();

    void print(std::ostream &stream) const override;

};

#endif //OBERON0C_UNDEFINEDTOKEN_H
