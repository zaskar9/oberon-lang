/*
 * Header file of the base class of the procedure symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_PROCEDURESYMBOL_H
#define OBERON0C_PROCEDURESYMBOL_H


#include <list>
#include "Symbol.h"
#include "ParameterSymbol.h"

class ProcedureSymbol : public Symbol {

private:
    std::list<std::shared_ptr<const ParameterSymbol>> parameters_;

public:
    explicit ProcedureSymbol(const std::string &name);
    ~ProcedureSymbol() override;

    void addParameter(std::shared_ptr<ParameterSymbol> parameter);

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_PROCEDURESYMBOL_H