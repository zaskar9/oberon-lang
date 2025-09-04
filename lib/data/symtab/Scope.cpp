/*
 * Scope of the symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/25/18.
 */

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

void Scope::insert(const std::string &name, DeclarationNode *symbol) {
    symbols_.push_back(symbol);
    auto idx = symbols_.size() - 1;
    indices_.insert(std::make_pair(name, idx));
}

DeclarationNode *Scope::lookup(const std::string &name, const bool local) const {
    auto itr = indices_.find(name);
    if (itr != indices_.end()) {
        const auto idx = itr->second;
        return symbols_.at(idx);
    }
    if (!local && parent_ != nullptr) {
        return parent_->lookup(name, local);
    }
    return nullptr;
}

void Scope::getExportedSymbols(std::vector<DeclarationNode *> &exports) const {
    for (const auto& symbol: symbols_) {
        const auto type = symbol->getNodeType();
        if (type == NodeType::constant || type == NodeType::variable || type == NodeType::type ||
            type == NodeType::field || type == NodeType::procedure) {
            const auto decl = dynamic_cast<DeclarationNode *>(symbol);
            if (decl->getIdentifier()->isExported()) {
                exports.push_back(decl);
            }
        }
    }
}