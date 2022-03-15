/*
 * Scope of the symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

#ifndef OBERON0C_SCOPE_H
#define OBERON0C_SCOPE_H


#include <memory>
#include <unordered_map>
#include <vector>
#include "../ast/DeclarationNode.h"

class Scope {

private:
    const unsigned int level_;
    std::unordered_map<std::string, Node *> symbols_;
    std::unique_ptr<Scope> child_;
    Scope *parent_;

public:
    explicit Scope(unsigned int level, Scope *parent) : level_(level), symbols_(), child_(), parent_(parent) {};
    ~Scope() = default;

    [[nodiscard]] unsigned int getLevel() const;

    [[nodiscard]] Scope *getParent() const;
    void setChild(std::unique_ptr<Scope> child);
    [[nodiscard]] Scope *getChild() const;

    void insert(const std::string &name, Node *symbol);
    [[nodiscard]] Node *lookup(const std::string &name, bool local) const;

    void getExportedSymbols(std::vector<DeclarationNode*> &exports) const;

};


#endif //OBERON0C_SCOPE_H
