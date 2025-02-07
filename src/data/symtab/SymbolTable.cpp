/*
 * Symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "SymbolTable.h"

#include <iostream>
#include <memory>
#include <string>

using std::make_unique;
using std::string;

const unsigned int SymbolTable::GLOBAL_SCOPE = 0;
const unsigned int SymbolTable::MODULE_SCOPE = 1;

SymbolTable::SymbolTable() : scopes_(), aliases_(), scope_(), references_() {
    universe_ = make_unique<Scope>(GLOBAL_SCOPE, nullptr);
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::import(const string &module, const string &name, DeclarationNode *node) {
    auto scope = getModule(module);
    if (scope) {
        node->setScope(MODULE_SCOPE);
        scope->insert(name, node);
    } else {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: namespace " + module + " does not exists." << std::endl;
        exit(1);
    }
}

void SymbolTable::setRef(unsigned ref, TypeNode *type) {
    auto idx = (size_t) ref;
    if (references_.size() <= idx) {
        references_.resize(idx + 1);
    }
    references_[idx] = type;
}

TypeNode *SymbolTable::getRef(unsigned ref) const {
    auto idx = (size_t) ref;
    return references_[idx];
}

void SymbolTable::insert(const string &name, DeclarationNode *node) {
#ifdef _DEBUG
    if (name.empty() || node == nullptr) {
        std::cerr << "Illegal symbol table state: trying to insert anonymous or null declaration." << std::endl;
        exit(1);
    }
#endif
    scope_->insert(name, node);
}

void SymbolTable::insertGlobal(const string &name, DeclarationNode *node) {
#ifdef _DEBUG
    if (name.empty() || node == nullptr) {
        std::cerr << "Illegal symbol table state: trying to insert anonymous or null declaration." << std::endl;
        exit(1);
    }
#endif
    universe_->insert(name, node);
}

DeclarationNode *SymbolTable::lookup(Ident *ident) const {
    if (ident->isQualified()) {
        return this->lookup(dynamic_cast<QualIdent*>(ident)->qualifier(), ident->name());
    }
    return this->lookup({}, ident->name());
}

DeclarationNode *SymbolTable::lookup(const string &qualifier, const string &name) const {
    if (!qualifier.empty()) {
        if (aliases_.contains(qualifier)) {
            string module = aliases_.find(qualifier)->second;
            auto it = scopes_.find(module);
            if (it != scopes_.end()) {
                return it->second->lookup(name, true);
            }
        }
        return nullptr;
    } else if (scope_) {
        auto node = scope_->lookup(name, false);
        if (node) {
            return node;
        }
    }
    return universe_->lookup(name, true);
}

void SymbolTable::addAlias(const string &alias, const string &module) {
    if (aliases_.contains(module)) {
        aliases_.erase(module);
    }
    aliases_[alias] = module;
}

bool SymbolTable::isDuplicate(const string &name) const {
    return scope_->lookup(name, true) != nullptr;
}

bool SymbolTable::isGlobal(const string &name) const {
    return universe_->lookup(name, true) != nullptr;
}

TypeNode *SymbolTable::getNilType() const {
    return nilType_;
}

void SymbolTable::setNilType(TypeNode *nilType) {
    nilType_ = nilType;
}

void SymbolTable::addModule(const string &module, bool activate) {
    if (getModule(module)) {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: namespace " + module + " already exists." << std::endl;
        exit(1);
    }
    auto scope = make_unique<Scope>(GLOBAL_SCOPE, nullptr);
    if (activate) {
        scope_ = scope.get();
    }
    scopes_[module] = std::move(scope);
    aliases_[module] = module;
}

Scope *SymbolTable::getModule(const string &module) {
    auto itr = scopes_.find(module);
    if (itr != scopes_.end()) {
        return itr->second.get();
    }
    return nullptr;
}

void SymbolTable::openScope() {
    auto child = make_unique<Scope>(scope_->getLevel() + 1, scope_);
    scope_->setChild(std::move(child));
    scope_ = scope_->getChild();
}

void SymbolTable::closeScope() {
    if (scope_->getLevel() > GLOBAL_SCOPE) {
        scope_ = scope_->getParent();
    } else {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: cannot leave current scope." << std::endl;
        exit(1);
    }
}

unsigned int SymbolTable::getLevel() const {
    if (scope_ == nullptr) {
        return GLOBAL_SCOPE;
    } else {
        return scope_->getLevel();
    }
}