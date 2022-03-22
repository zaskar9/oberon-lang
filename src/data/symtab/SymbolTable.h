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
#include "data/ast/TypeNode.h"
#include "logging/Logger.h"

/**
 * The symbol table manages the different lexical scopes of the compiled module as well as the scopes of the imported
 * modules. For each scope, the symbol table maps names to nodes of the abstract syntax tree.
 */
class SymbolTable {

private:
    std::unordered_map<std::string, std::unique_ptr<Scope>> scopes_;
    Scope *scope_;
    std::vector<std::unique_ptr<Node>> predefines_;
    std::unordered_map<char, TypeNode*> references_; // used for import and export
    std::unique_ptr<Scope> universe_;

    Node *basicType(const std::string &name, TypeKind kind, unsigned int size);

public:
    explicit SymbolTable();
    ~SymbolTable();

    void import(const std::string &module, const std::string &name, DeclarationNode *node);

    void setRef(char ref, TypeNode *type);
    TypeNode *getRef(char ref) const;

    void insert(const std::string &name, Node *node);

    [[nodiscard]] Node *lookup(const std::string &name) const;
    [[nodiscard]] Node *lookup(const std::string &qualifier, const std::string &name) const;
    [[nodiscard]] Node *lookup(Identifier *ident) const;

    [[nodiscard]] bool isDuplicate(const std::string &name) const;

    void createNamespace(const std::string &module, bool activate = false);
    Scope *getNamespace(const std::string &module);
    void setNamespace(const std::string &module);

    void openScope();
    void closeScope();

    [[nodiscard]] unsigned int getLevel() const;

    static const unsigned int GLOBAL_LEVEL;
    static const unsigned int MODULE_LEVEL;

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
