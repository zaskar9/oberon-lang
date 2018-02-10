/*
 * Header file of the base class of the array type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_ARRAYTYPESYMBOL_H
#define OBERON0C_ARRAYTYPESYMBOL_H


#include <memory>
#include "TypeSymbol.h"

class ArrayTypeSymbol : public TypeSymbol {

private:
    const int dim_;
    const std::shared_ptr<const TypeSymbol> mType_;

public:
    explicit ArrayTypeSymbol(int dim, std::shared_ptr<const TypeSymbol> mType);
    ~ArrayTypeSymbol() override;

    const int getDimension() const;
    const std::shared_ptr<const TypeSymbol> getMemberType() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
