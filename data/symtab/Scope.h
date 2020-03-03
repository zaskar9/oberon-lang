/*
 * Scope of the symtab table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

#ifndef OBERON0C_SCOPE_H
#define OBERON0C_SCOPE_H


#include <memory>
#include <unordered_map>
#include "../ast/Node.h"

class Scope {

private:
    std::unordered_map<std::string, Node*> symbols_;
    std::unique_ptr<Scope> parent_;

public:
    explicit Scope(std::unique_ptr<Scope> parent);
    ~Scope() = default;

    [[nodiscard]] std::unique_ptr<Scope> getParent();

    void insert(const std::string &name, Node* symbol);
    [[nodiscard]] Node* lookup(const std::string &name, bool local) const;

};


#endif //OBERON0C_SCOPE_H
