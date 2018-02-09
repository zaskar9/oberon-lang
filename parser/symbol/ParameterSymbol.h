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
    std::shared_ptr<const TypeSymbol> type_;
    bool var_;

public:
    explicit ParameterSymbol(const std::string &name, std::shared_ptr<const TypeSymbol> type, bool var);
    ~ParameterSymbol() override;

    std::shared_ptr<const TypeSymbol> getType() const;
    const bool isVar() const;

};

#endif //OBERON0C_PARAMETER_H
