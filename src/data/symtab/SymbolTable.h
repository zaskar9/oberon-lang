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

class SymbolTable {

private:
    unsigned int level_;
    std::unordered_map<std::string, std::unique_ptr<Scope>> scopes_;
    Scope *scope_;
    std::vector<std::unique_ptr<Node>> builtins;
    std::unique_ptr<Scope> universe_;

    Node* basicType(std::string name, TypeKind kind, unsigned int size);


public:
    explicit SymbolTable();
    ~SymbolTable();

    void insert(const std::string &name, Node *node);
    [[nodiscard]] Node* lookup(const std::string &name) const;
    [[nodiscard]] Node* lookup(const std::string &qualifier, const std::string &name) const;
    [[nodiscard]] Node* lookup(Identifier *ident) const;

    [[nodiscard]] bool isDuplicate(const std::string &name) const;

    void openNamespace(const std::string &module);

    void openScope();
    void closeScope();

    [[nodiscard]] unsigned int getLevel() const;

    static const std::string BOOLEAN;
    static const std::string BYTE;
    static const std::string CHAR;
    static const std::string INTEGER;
    static const std::string LONGINT;
    static const std::string REAL;
    static const std::string LONGREAL;
    static const std::string STRING;

};


#endif //OBERON0C_TABLE_H
