/*
 * Implementation of the base class of the procedure symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ProcedureSymbol.h"

ProcedureSymbol::ProcedureSymbol(const std::string &name) : Symbol(SymbolType::procedure, name), parameters_() {
}

ProcedureSymbol::~ProcedureSymbol() = default;

void ProcedureSymbol::addParameter(std::unique_ptr<const ParameterSymbol> parameter) {
    parameters_.push_back(std::move(parameter));
}

void ProcedureSymbol::print(std::ostream &out) const {
    out << "PROCEDURE " << getName();
}