/*
 * Header file of the class for the number constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#ifndef OBERON0C_NUMBERCONSTANTSYMBOL_H
#define OBERON0C_NUMBERCONSTANTSYMBOL_H


#include "ConstantSymbol.h"

class NumberConstantSymbol : public ConstantSymbol {

private:
    int value_;

public:
    explicit NumberConstantSymbol(const std::string &name, int value);
    ~NumberConstantSymbol() override;

    const int getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_NUMBERCONSTANTSYMBOL_H
