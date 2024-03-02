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

const unsigned int SymbolTable::GLOBAL_LEVEL = 0;
const unsigned int SymbolTable::MODULE_LEVEL = 1;

SymbolTable::SymbolTable() : scopes_(), scope_(), references_() {
    universe_ = make_unique<Scope>(GLOBAL_LEVEL, nullptr);
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::import(const string &module, const string &name, DeclarationNode *node) {
    auto scope = getNamespace(module);
    if (scope) {
        node->setLevel(MODULE_LEVEL);
        scope->insert(name, node);
    } else {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: namespace " + module + " does not exists." << std::endl;
        exit(1);
    }
}

void SymbolTable::setRef(char ref, TypeNode *type) {
    size_t idx = (size_t) ref;
    if (references_.size() <= idx) {
        references_.resize(idx + 1);
    }
    references_[idx] = type;
}

TypeNode *SymbolTable::getRef(char ref) const {
    size_t idx = (size_t) ref;
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

DeclarationNode *SymbolTable::lookup(const string &name) const {
    return this->lookup({}, name);
}

DeclarationNode *SymbolTable::lookup(const string &qualifier, const string &name) const {
    if (!qualifier.empty()) {
        auto it = scopes_.find(qualifier);
        if (it != scopes_.end()) {
            return it->second->lookup(name, true);
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

bool SymbolTable::isDuplicate(const string &name) const {
    return scope_->lookup(name, true) != nullptr;
}

bool SymbolTable::isGlobal(const string &name) const {
    return universe_->lookup(name, true) != nullptr;
}

BasicTypeNode *SymbolTable::getBasicType(const string &name) const {
    auto sym = universe_->lookup(name, true);
    if (sym && sym->getNodeType() == NodeType::type) {
        auto decl = dynamic_cast<TypeDeclarationNode *>(sym);
        return dynamic_cast<BasicTypeNode *>(decl->getType());
    }
    return nullptr;
}

TypeNode *SymbolTable::getNilType() const {
    return nilType_;
}

void SymbolTable::setNilType(TypeNode *nilType) {
    nilType_ = nilType;
}

void SymbolTable::createNamespace(const string &module, bool activate) {
    if (getNamespace(module)) {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: namespace " + module + " already exists." << std::endl;
        exit(1);
    }
    auto scope = make_unique<Scope>(GLOBAL_LEVEL, nullptr);
    if (activate) {
        scope_ = scope.get();
    }
    scopes_[module] = std::move(scope);
}

Scope *SymbolTable::getNamespace(const string &module) {
    auto itr = scopes_.find(module);
    if (itr != scopes_.end()) {
        return itr->second.get();
    }
    return nullptr;
}

void SymbolTable::setNamespace(const string &module) {
    auto scope = getNamespace(module);
    if (scope) {
        scope_ = scope;
    } else {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: namespace " + module + " does not exists." << std::endl;
        exit(1);
    }
}

void SymbolTable::openScope() {
    auto child = make_unique<Scope>(scope_->getLevel() + 1, scope_);
    scope_->setChild(std::move(child));
    scope_ = scope_->getChild();
}

void SymbolTable::closeScope() {
    if (scope_->getLevel() > GLOBAL_LEVEL) {
        scope_ = scope_->getParent();
    } else {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: cannot leave current scope." << std::endl;
        exit(1);
    }
}

unsigned int SymbolTable::getLevel() const {
    return scope_->getLevel();
}