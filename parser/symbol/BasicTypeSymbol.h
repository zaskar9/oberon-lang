/*
 * Implementation of the base class of the basic type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_BASICTYPESYMBOL_H
#define OBERON0C_BASICTYPESYMBOL_H


#include <string>
#include "TypeSymbol.h"

class BasicTypeSymbol : public TypeSymbol {

public:
    BasicTypeSymbol(const std::string &name, int size);
    ~BasicTypeSymbol() override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_BASICTYPESYMBOL_H
