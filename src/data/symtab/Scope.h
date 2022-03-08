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
#include "../ast/Node.h"

class Scope {

private:
    std::unordered_map<std::string, Node *> symbols_;
    std::vector<std::unique_ptr<Scope>> children_;
    Scope *parent_;

public:
    explicit Scope(Scope *parent) : symbols_(), children_(), parent_(parent) {};
    ~Scope() = default;

    [[nodiscard]] Scope *getParent();
    Scope *addChild(std::unique_ptr<Scope> child);

    void insert(const std::string &name, Node *symbol);
    [[nodiscard]] Node *lookup(const std::string &name, bool local) const;

};


#endif //OBERON0C_SCOPE_H
