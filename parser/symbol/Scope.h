/*
 * Header file of the symbol table scope used by the Oberon-0 compiler.
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
    Scope(std::unique_ptr<Scope> parent);
    ~Scope() = default;

    std::unique_ptr<Scope> getParent();

    void insert(const std::string &name, Node* symbol);
    Node* lookup(const std::string &name, bool local) const;

};

#endif //OBERON0C_SCOPE_H
