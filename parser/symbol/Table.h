/*
 * Header file of the symbol table class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TABLE_H
#define OBERON0C_TABLE_H

#include <string>
#include <unordered_map>
#include "Symbol.h"
#include "../../util/Logger.h"

class Table
{

private:
    std::unordered_map<std::string, const Symbol*> map_;
    Table *super_;
    Logger *log_;

    Table(Table *super, Logger *log);

public:
    explicit Table(Logger *log);
    ~Table();

    void insert(const Symbol *symbol);
    const Symbol* lookup(const std::string &name) const;
    Table openScope();
    Table closeScope() const;
    const bool isGlobal() const;

};


#endif //OBERON0C_TABLE_H
