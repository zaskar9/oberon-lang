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
#include "../ast/TypeNode.h"
#include "../../util/Logger.h"
#include "Scope.h"

class SymbolTable {

private:
    int level_;
    std::unique_ptr<Scope> scope_;

public:
    explicit SymbolTable();
    ~SymbolTable();

    void insert(const std::string &name, const Node *node);
    const Node* lookup(const std::string &name) const;
    bool isDefined(const std::string &name) const;
    bool isDuplicate(const std::string &name) const;
    void enterScope();
    void leaveScope();

};

#endif //OBERON0C_TABLE_H
