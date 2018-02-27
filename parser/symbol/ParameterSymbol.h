/*
 * Header file of the parameter symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_PARAMETER_H
#define OBERON0C_PARAMETER_H

#include "Symbol.h"
#include "../ast/TypeNode.h"

class ParameterSymbol : public Symbol {

private:
    const TypeNode* type_;
    const bool var_;

public:
    explicit ParameterSymbol(const std::string &name, const TypeNode* type, bool var);
    ~ParameterSymbol() override;

    const TypeNode* getType() const;
    const bool isVar() const;

    void print(std::ostream &stream) const override;

};

#endif //OBERON0C_PARAMETER_H
