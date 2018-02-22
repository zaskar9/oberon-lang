/*
 * Header file of the class for the constant symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/21/18.
 */

#ifndef OBERON0C_CONSTANTSYMBOL_H
#define OBERON0C_CONSTANTSYMBOL_H


#include "Symbol.h"

class ConstantSymbol : public Symbol {

public:
    explicit ConstantSymbol(SymbolType type, const std::string &name);
    ~ConstantSymbol() override;
};


#endif //OBERON0C_CONSTANTSYMBOL_H
