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
    int level_;
    std::unordered_map<std::string, std::shared_ptr<Node>> symbols_;
    std::unique_ptr<Scope> parent_;

public:
    Scope(std::unique_ptr<Scope> parent, const int level);
    ~Scope() = default;

    std::unique_ptr<Scope> getParent();

    void insert(const std::string &name, std::shared_ptr<Node> symbol);
    std::shared_ptr<Node> lookup(const std::string &name, bool local) const;

};

template<typename T>
T map_lookup(const std::unordered_map<std::string, T> &map, const std::string &name) {
    auto itr = map.find(name);
    if (itr == map.end()) {
        return nullptr;
    }
    return itr->second;
}

#endif //OBERON0C_SCOPE_H
