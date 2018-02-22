/*
 * Header file of the class for the string constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#ifndef OBERON0C_STRINGCONSTANTSYMBOL_H
#define OBERON0C_STRINGCONSTANTSYMBOL_H


#include "ConstantSymbol.h"

class StringConstantSymbol : public ConstantSymbol {

private:
    const std::string value_;

public:
    explicit StringConstantSymbol(const std::string &name, const std::string &value);
    ~StringConstantSymbol() override;

    const std::string getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_STRINGCONSTANTSYMBOL_H
