/*
 * Header file of the class for the Boolean constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#ifndef OBERON0C_BOOLEANCONSTANTSYMBOL_H
#define OBERON0C_BOOLEANCONSTANTSYMBOL_H


#include "ConstantSymbol.h"

class BooleanConstantSymbol : public ConstantSymbol  {

private:
    bool value_;

public:
    explicit BooleanConstantSymbol(const std::string &name, bool value);
    ~BooleanConstantSymbol() override;

    const bool getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_BOOLEANCONSTANTSYMBOL_H
