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
#include "../ast/TypeNode.h"
#include "../../util/Logger.h"

class SymbolTable
{

private:
    const SymbolTable *super_;
    std::unordered_map<std::string, const Node*> map_;
    std::unordered_map<std::string, std::shared_ptr<const TypeNode>> types_;

    explicit SymbolTable(const SymbolTable *super);

public:
    explicit SymbolTable();
    ~SymbolTable();

    void insert(const std::string &name, const Node* node);
    void insertType(const std::string &name, const std::shared_ptr<const TypeNode> &type);
    const Node* lookup(const std::string &name) const;
    const std::shared_ptr<const TypeNode> lookupType(const std::string &name) const;
    const bool exists(const std::string &name) const;
    const bool existsType(const std::string &name) const;
    std::unique_ptr<SymbolTable> openScope();

};

template<typename T>
T lookupLocal(const std::unordered_map<std::string, T> &map, const std::string &name) {
    auto itr = map.find(name);
    if (itr == map.end()) {
        return nullptr;
    }
    return itr->second;
}


#endif //OBERON0C_TABLE_H
