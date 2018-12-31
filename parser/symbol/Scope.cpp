/*
 * Implementation file of the symbol table scope used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

#include "Scope.h"


Scope::Scope(std::unique_ptr<Scope> parent) : symbols_(), parent_(std::move(parent)) {
}

std::unique_ptr<Scope> Scope::getParent() {
    return std::move(parent_);
}

void Scope::insert(const std::string &name, Node *symbol) {
    symbols_.insert(std::make_pair(name, symbol));
}

Node* Scope::lookup(const std::string &name, bool local) const {
    auto itr = symbols_.find(name);
    if (itr != symbols_.end()) {
        return itr->second;
    } else if (!local && parent_ != nullptr) {
        return parent_->lookup(name, local);
    } else {
        return nullptr;
    }
}