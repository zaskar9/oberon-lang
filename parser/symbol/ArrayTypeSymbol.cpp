/*
 * Implementation of the base class of the array type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeSymbol.h"

ArrayTypeSymbol::ArrayTypeSymbol(int dim, std::shared_ptr<const TypeSymbol> mType) :
        TypeSymbol(SymbolType::array_type, "", dim * mType->getSize()), dim_(dim), mType_(mType) {
}

ArrayTypeSymbol::~ArrayTypeSymbol() = default;

const int ArrayTypeSymbol::getDimension() const {
    return dim_;
}

const std::shared_ptr<const TypeSymbol> ArrayTypeSymbol::getMemberType() const {
    return mType_;
}

void ArrayTypeSymbol::print(std::ostream &out) const {
    out << "ARRAY " << dim_ << " OF " << mType_->getName();
}
