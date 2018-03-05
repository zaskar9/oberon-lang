/*
 * Header file of the base class of all symbols used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_SYMBOL_H
#define OBERON0C_SYMBOL_H

#include <memory>
#include <ostream>
#include <string>
#include "../ast/Node.h"

enum class SymbolType {
    type, variable, constant, procedure
};

class Symbol
{

private:
    SymbolType type_;
    const std::unique_ptr<const Node> node_;

public:
    explicit Symbol(SymbolType type, std::unique_ptr<const Node> node);
    ~Symbol();

    const SymbolType getType() const;
    const Node* getNode() const;

    void print(std::ostream &stream) const;
    friend std::ostream& operator<<(std::ostream &stream, const Symbol &symbol);
};


#endif //OBERON0C_SYMBOL_H
