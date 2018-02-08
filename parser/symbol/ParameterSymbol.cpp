/*
 * Implementation of the parameter symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "ParameterSymbol.h"

ParameterSymbol::ParameterSymbol(const std::string &name, const TypeSymbol *type, const bool var, const int pos) :
        Symbol(SymbolType::parameter, name), type_(type), var_(var), pos_(pos) {

}

ParameterSymbol::~ParameterSymbol() = default;

const int ParameterSymbol::getPos() const {
    return pos_;
}

const bool ParameterSymbol::isVar() const {
    return var_;
}
