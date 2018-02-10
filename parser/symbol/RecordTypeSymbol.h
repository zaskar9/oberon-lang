/*
 * Header file of the base class of the record type symbol used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_RECORDTYPESYMBOL_H
#define OBERON0C_RECORDTYPESYMBOL_H

#include <list>
#include "TypeSymbol.h"
#include "VariableSymbol.h"

class RecordTypeSymbol : public TypeSymbol {

private:
    std::list<std::shared_ptr<const VariableSymbol>> fields_;

public:
    explicit RecordTypeSymbol();
    ~RecordTypeSymbol() override;

    void addField(std::shared_ptr<const VariableSymbol> field);
    const int getSize() const override;

    void print(std::ostream &stream) const override;

};

#endif //OBERON0C_RECORDTYPESYMBOL_H
