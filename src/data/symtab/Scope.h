/*
 * Scope of the symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

#ifndef OBERON0C_SCOPE_H
#define OBERON0C_SCOPE_H


#include "data/ast/DeclarationNode.h"
#include <memory>
#include <unordered_map>
#include <vector>

class Scope {

private:
    const unsigned int level_;
    std::vector<DeclarationNode *> symbols_;
    std::unordered_map<std::string, size_t> indices_;
    std::unique_ptr<Scope> child_;
    Scope *parent_;

public:
    explicit Scope(unsigned int level, Scope *parent) :
            level_(level), symbols_(), indices_(), child_(), parent_(parent) {};
    ~Scope() = default;

    [[nodiscard]] unsigned int getLevel() const;

    [[nodiscard]] Scope *getParent() const;
    void setChild(std::unique_ptr<Scope> child);
    [[nodiscard]] Scope *getChild() const;

    void insert(const std::string &name, DeclarationNode *symbol);
    [[nodiscard]] DeclarationNode *lookup(const std::string &name, bool local) const;

    void getExportedSymbols(std::vector<DeclarationNode*> &exports) const;

};


#endif //OBERON0C_SCOPE_H
