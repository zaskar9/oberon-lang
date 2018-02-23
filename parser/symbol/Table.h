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
    Logger *logger_;
    std::unordered_map<std::string, std::unique_ptr<const Symbol>> map_;
    std::unique_ptr<Table> super_;

    Table(std::unique_ptr<Table> super, Logger *logger);

public:
    explicit Table(Logger *logger);
    ~Table();

    void insert(std::unique_ptr<const Symbol> symbol);
    void insert(const std::string &name, std::unique_ptr<const Symbol> symbol);
    const Symbol* lookup(const std::string &name) const;
    Table* openScope();
    Table* closeScope() const;
    const bool isGlobal() const;

};


#endif //OBERON0C_TABLE_H
