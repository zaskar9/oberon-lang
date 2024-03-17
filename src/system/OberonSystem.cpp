//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "OberonSystem.h"

#include<memory>
#include <string>
#include <utility>
#include <vector>

using std::make_unique;
using std::pair;
using std::string;
using std::vector;

OberonSystem::~OberonSystem() = default;

SymbolTable *OberonSystem::getSymbolTable() {
    if (symbols_ == nullptr) {
        symbols_ = make_unique<SymbolTable>();
        this->initSymbolTable(symbols_.get());
    }
    return symbols_.get();
}

void OberonSystem::createNamespace(const std::string &module) {
    module_ = module;
    symbols_->createNamespace(module, true);
    symbols_->openScope();
}

void OberonSystem::leaveNamespace() {
    module_ = "";
    symbols_->closeScope();
}

TypeDeclarationNode *OberonSystem::createTypeDeclaration(TypeNode *type) {
    auto decl = make_unique<TypeDeclarationNode>(EMPTY_POS, make_unique<IdentDef>(type->getIdentifier()->name()), type);
    auto ptr = decl.get();
    decls_.push_back(std::move(decl));
    return ptr;
}

void OberonSystem::createBasicTypes(const vector<pair<pair<TypeKind, unsigned int>, bool>>& types) {
    for (auto pair: types) {
        auto type = createBasicType(pair.first.first, pair.first.second);
        auto decl = createTypeDeclaration(type);
        if (pair.second) {
            symbols_->insertGlobal(type->getIdentifier()->name(), decl);
        }
    }
}

BasicTypeNode *OberonSystem::createBasicType(TypeKind kind, unsigned int size) {
    auto type = make_unique<BasicTypeNode>(make_unique<Ident>(to_string(kind)), kind, size);
    auto ptr = type.get();
    types_.push_back(std::move(type));
    symbols_->setRef((char) kind, ptr);
    baseTypes_[ptr->getIdentifier()->name()] = ptr;
    return ptr;
}

BasicTypeNode *OberonSystem::getBasicType(TypeKind kind) {
    return baseTypes_[to_string(kind)];
}

PointerTypeNode *OberonSystem::createPointerType(TypeNode *base) {
    auto type = make_unique<PointerTypeNode>(EMPTY_POS, nullptr, base);
    auto ptr = type.get();
    types_.push_back(std::move(type));
    symbols_->setRef((char) TypeKind::POINTER, ptr);
    return ptr;
}

ArrayTypeNode *OberonSystem::createArrayType(TypeNode *memberType, unsigned int dimension) {
    auto type = make_unique<ArrayTypeNode>(EMPTY_POS, nullptr, dimension, memberType);
    auto ptr = type.get();
    types_.push_back(std::move(type));
    symbols_->setRef((char) TypeKind::ARRAY, ptr);
    return ptr;
}

void OberonSystem::createProcedure(ProcKind kind, const string& name, const vector<pair<TypeNode *, bool>>& params,
                                   TypeNode *ret, bool varargs, bool toSymbols) {
    auto proc = make_unique<PredefinedProcedure>(kind, name, params, varargs, ret);
    auto ptr = proc.get();
    decls_.push_back(std::move(proc));
    if (toSymbols) {
        if (symbols_->getLevel() == SymbolTable::GLOBAL_LEVEL) {
            symbols_->insertGlobal(ptr->getIdentifier()->name(), ptr);
        } else {
            symbols_->import(module_, ptr->getIdentifier()->name(), ptr);
        }
    }
}

void Oberon07::initSymbolTable(SymbolTable *symbols) {

    this->createBasicTypes(
            {
                    {{TypeKind::ANYTYPE,  0}, false},
                    {{TypeKind::NOTYPE,   0}, false},
                    {{TypeKind::NILTYPE,  8}, false},
                    {{TypeKind::ENTIRE,   0}, false},
                    {{TypeKind::BOOLEAN,  1}, true},
                    {{TypeKind::BYTE,     1}, true},
                    {{TypeKind::CHAR,     1}, true},
                    {{TypeKind::INTEGER,  4}, true},
                    {{TypeKind::LONGINT,  8}, true},
                    {{TypeKind::REAL,     4}, true},
                    {{TypeKind::LONGREAL, 8}, true},
                    {{TypeKind::SET,      4}, true},
                    {{TypeKind::STRING,   8}, false},
                    {{TypeKind::TYPE,     0}, false}
            }
    );

    symbols->setNilType(this->getBasicType(TypeKind::NILTYPE));
    auto anyType = this->getBasicType(TypeKind::ANYTYPE);
    auto entireType = this->getBasicType(TypeKind::ENTIRE);
    auto boolType = this->getBasicType(TypeKind::BOOLEAN);
    auto intType = this->getBasicType(TypeKind::INTEGER);
    auto longType = this->getBasicType(TypeKind::LONGINT);
    auto setType = this->getBasicType(TypeKind::SET);
    auto charType = this->getBasicType(TypeKind::CHAR);
    auto typeType = this->getBasicType(TypeKind::TYPE);

    this->createProcedure(ProcKind::NEW, "NEW", {{this->createPointerType(anyType), true}}, nullptr, false, true);
    this->createProcedure(ProcKind::FREE, "FREE", {{this->createPointerType(anyType), true}}, nullptr, false, true);
    this->createProcedure(ProcKind::INC, "INC", {{entireType, true}}, nullptr, true, true);
    this->createProcedure(ProcKind::DEC, "DEC", {{entireType, true}}, nullptr, true, true);
    this->createProcedure(ProcKind::LSL, "LSL", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcKind::ASR, "ASR", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcKind::ROL, "ROL", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcKind::ROR, "ROR", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcKind::ODD, "ODD", {{longType, false}}, boolType, false, true);
    this->createProcedure(ProcKind::HALT, "HALT", {{intType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::ASSERT, "ASSERT", {{boolType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::LEN, "LEN", {{this->createArrayType(anyType, 0), false}}, longType, true, true);
    this->createProcedure(ProcKind::INCL, "INCL", {{setType, true}, {intType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::EXCL, "EXCL", {{setType, true}, {intType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::ORD, "ORD", {{anyType, false}}, intType, false, true);
    this->createProcedure(ProcKind::CHR, "CHR", {{intType, false}}, charType, false, true);
    this->createProcedure(ProcKind::SIZE, "SIZE", {{typeType, false}}, longType, false, true);

    createNamespace("SYSTEM");
    this->createProcedure(ProcKind::SYSTEM_ADR, "ADR", {{anyType, true}}, longType, false, true);
    this->createProcedure(ProcKind::SYSTEM_GET, "GET", {{longType, false}, {anyType, true}}, nullptr, false, true);
    this->createProcedure(ProcKind::SYSTEM_PUT, "PUT", {{longType, false}, {anyType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::SYSTEM_BIT, "BIT", {{longType, false}, {intType, false}}, boolType, false, true);
    this->createProcedure(ProcKind::SYSTEM_COPY, "COPY", {{longType, false}, {longType, false}, {longType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::SYSTEM_SIZE, "SIZE", {{typeType, false}}, longType, false, true);
    leaveNamespace();
}
