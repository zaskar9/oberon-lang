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
#include <vector>

#include "Logger.h"
#include "Scope.h"
#include "data/ast/TypeNode.h"
#include "data/ast/ProcedureNode.h"
#include "data/ast/PointerTypeNode.h"

using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

/**
 * The symbol table manages the different lexical scopes of the compiled module as well as the scopes of the imported
 * modules. For each scope, the symbol table maps names to nodes of the abstract syntax tree.
 */
class SymbolTable {

public:
    explicit SymbolTable();
    ~SymbolTable();

    void import(const string &module, const string &name, DeclarationNode *node);

    void setRef(unsigned ref, TypeNode *type);
    [[nodiscard]] TypeNode *getRef(unsigned ref) const;

    void insert(const string &name, DeclarationNode *node) const;
    void insertGlobal(const string &name, DeclarationNode *node) const;

    [[nodiscard]] DeclarationNode *lookup(const string &qualifier, const string &name) const;
    [[nodiscard]] DeclarationNode *lookup(Ident *ident) const;

    void addAlias(const string &alias, const string &module);

    [[nodiscard]] bool isDuplicate(const string &name) const;
    [[nodiscard]] bool isGlobal(const string &name) const;

    [[nodiscard]] TypeNode *getNilType() const;
    void setNilType(TypeNode *nilType);

    void addModule(const string &module, bool activate = false);
    [[nodiscard]] Scope *getModule(const string &module);

    void openScope();
    void closeScope();

    [[nodiscard]] unsigned int getLevel() const;

    static const unsigned int GLOBAL_SCOPE;
    static const unsigned int MODULE_SCOPE;

private:
    unordered_map<string, unique_ptr<Scope>> scopes_;
    unordered_map<string, string> aliases_;
    Scope *scope_;
    // references for import and export
    unordered_map<unsigned, TypeNode*> references_;
    unique_ptr<Scope> universe_;
    TypeNode *nilType_{};
};


#endif //OBERON0C_TABLE_H
