/*
 * Header file of the number tokens used by parser of the Oberon-0 compiler.
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
    NumberToken(FilePos pos, int value);
    ~NumberToken() override;

    const int getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_NUMBERTOKEN_H
