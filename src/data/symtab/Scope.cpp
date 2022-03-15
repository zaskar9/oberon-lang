/*
 * Scope of the symtab table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

#include <iostream>
#include "Scope.h"

unsigned int Scope::getLevel() const {
    return level_;
}

Scope *Scope::getParent() const {
    return parent_;
}

void Scope::setChild(std::unique_ptr<Scope> child) {
    child_ = std::move(child);
}

Scope *Scope::getChild() const {
    return child_.get();
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

void Scope::getExportedSymbols(std::vector<DeclarationNode *> &exports) const {
    for (const auto& entry: symbols_) {
        auto symbol = entry.second;
        auto type = symbol->getNodeType();
        if (type == NodeType::constant || type == NodeType::variable || type == NodeType::type_declaration ||
            type == NodeType::field || type == NodeType::procedure) {
            auto decl = dynamic_cast<DeclarationNode *>(symbol);
            if (decl->getIdentifier()->isExported()) {
                exports.push_back(decl);
            }
        }
    }
}