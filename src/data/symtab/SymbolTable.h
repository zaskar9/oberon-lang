/*
 * Symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TABLE_H
#define OBERON0C_TABLE_H


#include "Scope.h"
#include "data/ast/TypeNode.h"
#include "logging/Logger.h"
#include "data/ast/ProcedureNode.h"
#include "data/ast/PointerTypeNode.h"
#include <memory>
#include <string>
#include <unordered_map>

/**
 * The symbol table manages the different lexical scopes of the compiled module as well as the scopes of the imported
 * modules. For each scope, the symbol table maps names to nodes of the abstract syntax tree.
 */
class SymbolTable {

private:
    std::unordered_map<std::string, std::unique_ptr<Scope>> scopes_;
    Scope *scope_;
    // all predefines are collected for memory-management purposes
    std::vector<std::unique_ptr<Node>> predefines_;
    // references for import and export
    std::vector<TypeNode*> references_;
    std::unique_ptr<Scope> universe_;
    TypeNode *nilType_;

    BasicTypeNode *basicType(const std::string &name, TypeKind kind, unsigned int size);
    PointerTypeNode *pointerType(TypeNode *base);
    ProcedureNode *procedure(const std::string &name, std::vector<std::pair<TypeNode*, bool>> params);

public:
    explicit SymbolTable();
    ~SymbolTable();

    void import(const std::string &module, const std::string &name, DeclarationNode *node);

    void setRef(char ref, TypeNode *type);
    [[nodiscard]] TypeNode *getRef(char ref) const;

    void insert(const std::string &name, Node *node);

    [[nodiscard]] Node *lookup(const std::string &name) const;
    [[nodiscard]] Node *lookup(const std::string &qualifier, const std::string &name) const;
    [[nodiscard]] Node *lookup(Ident *ident) const;

    [[nodiscard]] bool isDuplicate(const std::string &name) const;

    [[nodiscard]] TypeNode *getNilType() const;

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
