/*
 * Header file of the base class of the variable symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_VARIABLESYMBOL_H
#define OBERON0C_VARIABLESYMBOL_H


#include <string>
#include "TypeSymbol.h"

class VariableSymbol : public Symbol {

private:
    std::shared_ptr<const TypeSymbol> type_;

public:
    VariableSymbol(const std::string &name, std::shared_ptr<const TypeSymbol> type);
    ~VariableSymbol() override;

    const std::shared_ptr<const TypeSymbol> getType() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_VARIABLESYMBOL_H
