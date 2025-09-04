/*
 * Scope of the symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

#ifndef OBERON0C_SCOPE_H
#define OBERON0C_SCOPE_H


#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "data/ast/DeclarationNode.h"

using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

class Scope {

public:
    Scope(const unsigned int level, Scope *parent) : level_(level), parent_(parent) {}
    ~Scope() = default;

    [[nodiscard]] unsigned int getLevel() const;

    [[nodiscard]] Scope *getParent() const;
    void setChild(std::unique_ptr<Scope> child);
    [[nodiscard]] Scope *getChild() const;

    void insert(const std::string &name, DeclarationNode *symbol);
    [[nodiscard]] DeclarationNode *lookup(const std::string &name, bool local) const;

    void getExportedSymbols(std::vector<DeclarationNode*> &exports) const;

private:
    const unsigned int level_;
    vector<DeclarationNode *> symbols_;
    unordered_map<string, size_t> indices_;
    std::unique_ptr<Scope> child_;
    Scope *parent_;

};


#endif //OBERON0C_SCOPE_H
