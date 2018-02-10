/*
 * Header file of the base class of all symbols used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_SYMBOL_H
#define OBERON0C_SYMBOL_H

#include <ostream>
#include <string>

enum class SymbolType {
    array_type, record_type, basic_type, boolean_const, number_const, variable, parameter, procedure
};

class Symbol
{

private:
    SymbolType type_;
    std::string name_;

public:
    explicit Symbol(SymbolType type, const std::string &name);
    virtual ~Symbol();

    const SymbolType getSymbolType() const;
    const std::string getName() const;

    virtual void print(std::ostream &stream) const = 0;
    friend std::ostream& operator<<(std::ostream &stream, const Symbol &symbol);
};


#endif //OBERON0C_SYMBOL_H
