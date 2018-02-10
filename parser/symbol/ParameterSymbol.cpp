/*
 * Implementation of the parameter symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "ParameterSymbol.h"

ParameterSymbol::ParameterSymbol(const std::string &name, const std::shared_ptr<const TypeSymbol> type, const bool var) :
        Symbol(SymbolType::parameter, name), type_(type), var_(var) {
}

ParameterSymbol::~ParameterSymbol() = default;

std::shared_ptr<const TypeSymbol> ParameterSymbol::getType() const {
    return type_;
}

const bool ParameterSymbol::isVar() const {
    return var_;
}

void ParameterSymbol::print(std::ostream &out) const {
    out << (var_ ? "VAR " : "") << getName() << ": " << type_->getName();
}
