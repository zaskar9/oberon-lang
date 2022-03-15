/*
 * Symbol table used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "SymbolTable.h"

const unsigned int SymbolTable::GLOBAL_LEVEL = 0;
const unsigned int SymbolTable::MODULE_LEVEL = 1;

const std::string SymbolTable::BOOLEAN = "BOOLEAN";
const std::string SymbolTable::BYTE = "BYTE";
const std::string SymbolTable::CHAR = "CHAR";
const std::string SymbolTable::INTEGER = "INTEGER";
const std::string SymbolTable::LONGINT = "LONGINT";
const std::string SymbolTable::REAL = "REAL";
const std::string SymbolTable::LONGREAL = "LONGREAL";
const std::string SymbolTable::STRING = "STRING";

SymbolTable::SymbolTable() : scopes_(), scope_(), builtins() {
    universe_ = std::make_unique<Scope>(GLOBAL_LEVEL, nullptr);
    basicType(SymbolTable::BOOLEAN, TypeKind::BOOLEAN, 1);
    basicType(SymbolTable::BYTE, TypeKind::BYTE, 1);
    basicType(SymbolTable::CHAR, TypeKind::CHAR, 1);
    auto type = basicType(SymbolTable::INTEGER, TypeKind::INTEGER, 4);
    universe_->insert(SymbolTable::LONGINT, type);
    type = basicType(SymbolTable::REAL, TypeKind::REAL, 4);
    universe_->insert(SymbolTable::LONGREAL, type);
    basicType(SymbolTable::STRING, TypeKind::STRING, 8);
}

SymbolTable::~SymbolTable() = default;

Node *SymbolTable::basicType(const std::string &name, TypeKind kind, unsigned int size) {
    auto type = std::make_unique<BasicTypeNode>(std::make_unique<Identifier>(name), kind, size);
    auto ptr = type.get();
    universe_->insert(name, ptr);
    builtins.push_back(std::move(type));
    return ptr;
}

void SymbolTable::insert(const std::string &name, Node *node) {
    scope_->insert(name, node);
}

Node *SymbolTable::lookup(Identifier *ident) const {
    return this->lookup(ident->qualifier(), ident->name());
}

Node *SymbolTable::lookup(const std::string &name) const {
    return this->lookup({}, name);
}

Node *SymbolTable::lookup(const std::string &qualifier, const std::string &name) const {
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

bool SymbolTable::isDuplicate(const std::string &name) const {
    return scope_->lookup(name, true) != nullptr;
}

Scope *SymbolTable::openNamespace(const std::string &module) {
    auto itr = scopes_.find(module);
    if (itr != scopes_.end()) {
        scope_ = itr->second.get();
    } else {
        auto scope = std::make_unique<Scope>(GLOBAL_LEVEL, nullptr);
        scope_ = scope.get();
        scopes_[module] = std::move(scope);
    }
    return scope_;
}

void SymbolTable::openScope() {
    auto child = std::make_unique<Scope>(scope_->getLevel() + 1, scope_);
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