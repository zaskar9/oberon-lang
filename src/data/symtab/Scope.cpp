/*
 * Scope of the symtab table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

#include "Scope.h"

Scope *Scope::getParent() {
    return parent_;
}

Scope *Scope::addChild(std::unique_ptr<Scope> child) {
    auto result = child.get();
    children_.push_back(std::move(child));
    return result;
}

void Scope::insert(const std::string &name, Node *symbol) {
    symbols_.insert(std::make_pair(name, symbol));
}

Node *Scope::lookup(const std::string &name, bool local) const {
    auto itr = symbols_.find(name);
    if (itr != symbols_.end()) {
        return itr->second;
    } else if (!local && parent_ != nullptr) {
        return parent_->lookup(name, local);
    } else {
        return nullptr;
    }
}