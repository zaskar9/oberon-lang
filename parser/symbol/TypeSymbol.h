/*
 * Header file of the base class of the type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TYPESYMBOL_H
#define OBERON0C_TYPESYMBOL_H

#include "Symbol.h"

class TypeSymbol : public Symbol {

private:
    int size_;

public:
    explicit TypeSymbol(SymbolType type, const std::string &name, int size);
    ~TypeSymbol() override;

    virtual const int getSize() const;

};


#endif //OBERON0C_TYPESYMBOL_H
