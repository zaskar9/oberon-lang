/*
 * Implementation of the base class of the record type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "RecordTypeSymbol.h"

RecordTypeSymbol::RecordTypeSymbol() : TypeSymbol(SymbolType::record_type, "", 0) {
}

RecordTypeSymbol::~RecordTypeSymbol() = default;

void RecordTypeSymbol::addField(std::shared_ptr<const VariableSymbol> field) {
    fields_.push_back(field);
}

const int RecordTypeSymbol::getSize() const {
    int size = 0;
    for (auto itr : fields_) {
        size += itr->getType()->getSize();
    }
    return size;
}

void RecordTypeSymbol::print(std::ostream &out) const {
    out << "RECORD";
}