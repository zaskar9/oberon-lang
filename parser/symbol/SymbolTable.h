/*
 * Header file of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TABLE_H
#define OBERON0C_TABLE_H

#include <memory>
#include <string>
#include <unordered_map>
#include "Symbol.h"
#include "../../util/Logger.h"

class SymbolTable
{

private:
    const SymbolTable *super_;
    std::unordered_map<std::string, std::unique_ptr<const Symbol>> map_;

    explicit SymbolTable(const SymbolTable *super);

public:
    explicit SymbolTable();
    ~SymbolTable();

    void insert(const std::string &name, std::unique_ptr<const Symbol> symbol);
    const Symbol* lookup(const std::string &name) const;
    const bool exists(const std::string &name) const;
    std::unique_ptr<SymbolTable> openScope();

};


#endif //OBERON0C_TABLE_H
