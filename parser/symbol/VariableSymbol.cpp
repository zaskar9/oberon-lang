/*
 * Implementation of the base class of the variable symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "VariableSymbol.h"

VariableSymbol::VariableSymbol(const std::string &name, const std::shared_ptr<const TypeSymbol> type) :
        Symbol(SymbolType::variable, name), type_(type) {
}

VariableSymbol::~VariableSymbol() = default;

const std::shared_ptr<const TypeSymbol> VariableSymbol::getType() const {
    return type_;
}

void VariableSymbol::print(std::ostream &out) const {
    out << getName() << ": " << type_->getName();
}
