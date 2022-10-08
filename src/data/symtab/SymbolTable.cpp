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

SymbolTable::SymbolTable() : scopes_(), scope_(), predefines_(), references_() {
    universe_ = std::make_unique<Scope>(GLOBAL_LEVEL, nullptr);
    auto anyType = basicType("ANYTYPE", TypeKind::ANYTYPE, 0);
    basicType("NOTYPE", TypeKind::NOTYPE, 0);
    nilType_ = basicType("NILTYPE", TypeKind::NILTYPE, 8);
    auto type = basicType(SymbolTable::BOOLEAN, TypeKind::BOOLEAN, 1);
    universe_->insert(SymbolTable::BOOLEAN, type);
    type = basicType(SymbolTable::BYTE, TypeKind::BYTE, 1);
    universe_->insert(SymbolTable::BYTE, type);
    type = basicType(SymbolTable::CHAR, TypeKind::CHAR, 1);
    universe_->insert(SymbolTable::CHAR, type);
    type = basicType(SymbolTable::INTEGER, TypeKind::INTEGER, 4);
    universe_->insert(SymbolTable::INTEGER, type);
    type = basicType(SymbolTable::LONGINT, TypeKind::LONGINT, 8);
    universe_->insert(SymbolTable::LONGINT, type);
    type = basicType(SymbolTable::REAL, TypeKind::REAL, 4);
    universe_->insert(SymbolTable::REAL, type);
    type = basicType(SymbolTable::LONGREAL, TypeKind::LONGREAL, 8);
    universe_->insert(SymbolTable::LONGREAL, type);
    type = basicType(SymbolTable::STRING, TypeKind::STRING, 8);
    universe_->insert(SymbolTable::STRING, type);
    auto proc = procedure("NEW", { { pointerType(anyType), true } });
    universe_->insert(proc->getIdentifier()->name(), proc);
}

SymbolTable::~SymbolTable() = default;

BasicTypeNode *SymbolTable::basicType(const std::string &name, TypeKind kind, unsigned int size) {
    auto type = std::make_unique<BasicTypeNode>(std::make_unique<Ident>(name), kind, size);
    auto ptr = type.get();
    predefines_.push_back(std::move(type));
    setRef((char) kind, ptr);
    return ptr;
}

PointerTypeNode *SymbolTable::pointerType(TypeNode *base) {
    auto type = std::make_unique<PointerTypeNode>(EMPTY_POS, nullptr, base);
    auto ptr = type.get();
    predefines_.push_back(std::move(type));
    setRef((char) TypeKind::POINTER, ptr);
    return ptr;
}

ProcedureNode *SymbolTable::procedure(const std::string &name, std::vector<std::pair<TypeNode *, bool>> params) {
    auto proc = std::make_unique<ProcedureNode>(EMPTY_POS, std::make_unique<Ident>(name));
    for (auto param : params) {
        proc->addFormalParameter(
                std::make_unique<ParameterNode>(EMPTY_POS, std::make_unique<Ident>("_"), param.first,
                                                param.second));
    }
    auto ptr = proc.get();
    predefines_.push_back(std::move(proc));
    return ptr;
}

void SymbolTable::import(const std::string &module, const std::string &name, DeclarationNode *node) {
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

void SymbolTable::insert(const std::string &name, Node *node) {
    scope_->insert(name, node);
}

Node *SymbolTable::lookup(Ident *ident) const {
    if (ident->isQualified()) {
        return this->lookup(dynamic_cast<QualIdent*>(ident)->qualifier(), ident->name());
    }
    return this->lookup({}, ident->name());
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

TypeNode *SymbolTable::getNilType() const {
    return nilType_;
}

void SymbolTable::createNamespace(const std::string &module, bool activate) {
    if (getNamespace(module)) {
        // TODO throw exception
        std::cerr << "Illegal symbol table state: namespace " + module + " already exists." << std::endl;
        exit(1);
    }
    auto scope = std::make_unique<Scope>(GLOBAL_LEVEL, nullptr);
    if (activate) {
        scope_ = scope.get();
    }
    scopes_[module] = std::move(scope);
}

Scope *SymbolTable::getNamespace(const std::string &module) {
    auto itr = scopes_.find(module);
    if (itr != scopes_.end()) {
        return itr->second.get();
    }
    return nullptr;
}

void SymbolTable::setNamespace(const std::string &module) {
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