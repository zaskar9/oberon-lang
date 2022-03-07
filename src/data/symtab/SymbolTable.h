/*
 * Symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TABLE_H
#define OBERON0C_TABLE_H


#include <memory>
#include <string>
#include <unordered_map>
#include "Scope.h"
#include "../ast/TypeNode.h"
#include "../../util/Logger.h"

// TODO Use Identifier instead of std::string
class SymbolTable {

private:
    unsigned int level_;
    std::unique_ptr<Scope> scope_;

public:
    explicit SymbolTable();
    ~SymbolTable();

    void insert(const std::string &name, Node *node);
    [[nodiscard]] Node* lookup(const std::string &name) const;

    [[nodiscard]] bool isDefined(const std::string &name) const;
    [[nodiscard]] bool isDuplicate(const std::string &name) const;

    void enterScope();
    void leaveScope();

    [[nodiscard]] unsigned int getLevel() const;

};


#endif //OBERON0C_TABLE_H
