/*
 * Header file of the parameter symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_PARAMETER_H
#define OBERON0C_PARAMETER_H

#include "Symbol.h"
#include "TypeSymbol.h"

class ParameterSymbol : public Symbol {

private:
    const TypeSymbol *type_;
    bool var_;
    int pos_;

public:
    explicit ParameterSymbol(const std::string &name, const TypeSymbol *type, const bool var, const int pos);
    ~ParameterSymbol() override;

    const int getPos() const;
    const bool isVar() const;

};

#endif //OBERON0C_PARAMETER_H
