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
    symbols_->addModule(module, true);
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
    symbols_->setRef(static_cast<unsigned char>(kind), ptr);
    baseTypes_[ptr->getIdentifier()->name()] = ptr;
    return ptr;
}

BasicTypeNode *OberonSystem::getBasicType(TypeKind kind) {
    return baseTypes_[to_string(kind)];
}

PointerTypeNode *OberonSystem::createPointerType(TypeNode *base) {
    auto type = make_unique<PointerTypeNode>(EMPTY_POS, base);
    auto ptr = type.get();
    types_.push_back(std::move(type));
    symbols_->setRef(static_cast<unsigned char>(TypeKind::POINTER), ptr);
    return ptr;
}

ArrayTypeNode *OberonSystem::createArrayType(const vector<unsigned> &dimensions, const vector<TypeNode *> &types) {
    auto type = make_unique<ArrayTypeNode>(EMPTY_POS, dimensions.size(), dimensions, types);
    auto ptr = type.get();
    types_.push_back(std::move(type));
    symbols_->setRef(static_cast<unsigned char>(TypeKind::ARRAY), ptr);
    return ptr;
}

PredefinedProcedure*
OberonSystem::createProcedure(ProcKind kind, const string& name, const vector<pair<TypeNode *, bool>>& params,
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
    return ptr;
}

void Oberon07::initSymbolTable(SymbolTable *symbols) {

    this->createBasicTypes(
            {
                    {{TypeKind::ANYTYPE,  0}, false},
                    {{TypeKind::NOTYPE,   0}, false},
                    {{TypeKind::TYPE,     0}, false},
                    {{TypeKind::NUMERIC,  0}, false},
                    {{TypeKind::ENTIRE,   0}, false},
                    {{TypeKind::FLOATING, 0}, false},
                    {{TypeKind::NILTYPE,  8}, false},
                    {{TypeKind::STRING,   8}, false},
                    {{TypeKind::BOOLEAN,  1}, true},
                    {{TypeKind::BYTE,     1}, true},
                    {{TypeKind::CHAR,     1}, true},
                    {{TypeKind::SHORTINT, 2}, true},
                    {{TypeKind::INTEGER,  4}, true},
                    {{TypeKind::LONGINT,  8}, true},
                    {{TypeKind::REAL,     4}, true},
                    {{TypeKind::LONGREAL, 8}, true},
                    {{TypeKind::SET,      4}, true},
            }
    );

    // Type of the pointer literal `NIL`
    symbols->setNilType(this->getBasicType(TypeKind::NILTYPE));

    // Virtual types required by the semantic analyzer to perform type inference
    // - `ANYTYPE`: matches a value of any type, used to simulate polymorphism for (built-in) procedures
    // - `NOTYPE`: matches no value of any type, used to indicate a failure state in type inference
    auto anyType = this->getBasicType(TypeKind::ANYTYPE);
    auto nullType = this->getBasicType(TypeKind::NOTYPE);

    // Virtual compound types to narrow the scope of possibilities during type inference
    // - `TYPE`: matches a value of that is a type
    // - `NUMERIC`: matches all numeric values
    // - `ENTIRE`: matches all integer values
    // - `FLOATING`: matches all floating-point values
    auto typeType = this->getBasicType(TypeKind::TYPE);
    auto numType = this->getBasicType(TypeKind::NUMERIC);
    auto entireType = this->getBasicType(TypeKind::ENTIRE);
    auto floatType = this->getBasicType(TypeKind::FLOATING);

    // Concrete Oberon data types that are visible to and usable by the programmer
    auto boolType = this->getBasicType(TypeKind::BOOLEAN);
    auto charType = this->getBasicType(TypeKind::CHAR);
    auto shortIntType = this->getBasicType(TypeKind::SHORTINT);
    auto intType = this->getBasicType(TypeKind::INTEGER);
    auto longIntType = this->getBasicType(TypeKind::LONGINT);
    auto realType = this->getBasicType(TypeKind::REAL);
    auto longRealType = this->getBasicType(TypeKind::LONGREAL);
    auto setType = this->getBasicType(TypeKind::SET);

    // NEW
    this->createProcedure(ProcKind::NEW, "NEW", {{this->createPointerType(anyType), true}}, nullptr, false, true);
    // DISPOSE
    this->createProcedure(ProcKind::DISPOSE, "DISPOSE", {{this->createPointerType(anyType), true}}, nullptr, false, true);
    // INC
    this->createProcedure(ProcKind::INC, "INC", {{entireType, true}}, nullptr, true, true);
    // DEC
    this->createProcedure(ProcKind::DEC, "DEC", {{entireType, true}}, nullptr, true, true);
    // LSL
    auto proc = this->createProcedure(ProcKind::LSL, "LSL", {{longIntType, false}, {entireType, false}}, longIntType, false, true);
    proc->overload({{intType, false}, {entireType, false}}, false, intType);
    proc->overload({{shortIntType, false}, {entireType, false}}, false, shortIntType);
    // ASR
    proc = this->createProcedure(ProcKind::ASR, "ASR", {{longIntType, false}, {entireType, false}}, longIntType, false, true);
    proc->overload({{intType, false}, {entireType, false}}, false, intType);
    proc->overload({{shortIntType, false}, {entireType, false}}, false, shortIntType);
    // ROR
    proc = this->createProcedure(ProcKind::ROR, "ROR", {{longIntType, false}, {entireType, false}}, longIntType, false, true);
    proc->overload({{intType, false}, {entireType, false}}, false, intType);
    proc->overload({{shortIntType, false}, {entireType, false}}, false, shortIntType);
    // ODD
    this->createProcedure(ProcKind::ODD, "ODD", {{longIntType, false}}, boolType, false, true);
    // HALT
    this->createProcedure(ProcKind::HALT, "HALT", {{intType, false}}, nullptr, false, true);
    // ASSERT
    this->createProcedure(ProcKind::ASSERT, "ASSERT", {{boolType, false}}, nullptr, false, true);
    // LEN
    this->createProcedure(ProcKind::LEN, "LEN", {{this->createArrayType({0}, {anyType}), false}}, longIntType, true, true);
    // INCL
    this->createProcedure(ProcKind::INCL, "INCL", {{setType, true}, {intType, false}}, nullptr, false, true);
    // EXCL
    this->createProcedure(ProcKind::EXCL, "EXCL", {{setType, true}, {intType, false}}, nullptr, false, true);
    // ORD
    this->createProcedure(ProcKind::ORD, "ORD", {{anyType, false}}, intType, false, true);
    // CHR
    this->createProcedure(ProcKind::CHR, "CHR", {{intType, false}}, charType, false, true);
    // SIZE
    this->createProcedure(ProcKind::SIZE, "SIZE", {{typeType, false}}, longIntType, false, true);
    // SHORT
    proc = this->createProcedure(ProcKind::SHORT, "SHORT", {{numType, false}}, nullType, false, true);
    proc->overload({{longIntType, false}}, false, intType);
    proc->overload({{intType, false}}, false, shortIntType);
    proc->overload({{longRealType, false}}, false, realType);
    // LONG
    proc = this->createProcedure(ProcKind::LONG, "LONG", {{numType, false}}, nullType, false, true);
    proc->overload({{shortIntType, false}}, false, intType);
    proc->overload({{intType, false}}, false, longIntType);
    proc->overload({{realType, false}}, false, longRealType);
    // ENTIER
    this->createProcedure(ProcKind::ENTIER, "ENTIER", {{floatType, false}}, longIntType, false, true);
    // FLOOR
    this->createProcedure(ProcKind::ENTIER, "FLOOR", {{floatType, false}}, longIntType, false, true);
    // FLT
    this->createProcedure(ProcKind::FLT, "FLT", {{longIntType, false}}, longRealType, false, true);
    // PACK
    proc = this->createProcedure(ProcKind::PACK, "PACK", {{floatType, false}, {entireType, false}}, nullType, false, true);
    proc->overload({{realType, false}, {entireType, false}}, false, realType);
    proc->overload({{longRealType, false}, {entireType, false}}, false, longRealType);
    // UNPK
    proc = this->createProcedure(ProcKind::UNPK, "UNPK", {{floatType, true}, {intType, true}}, nullType, false, true);
    proc->overload({{realType, true}, {intType, true}}, false, nullType);
    proc->overload({{longRealType, true}, {intType, true}}, false, nullType);
    // ABS
    proc = this->createProcedure(ProcKind::ABS, "ABS", {{numType, false}}, nullType, false, true);
    proc->overload({{shortIntType, false}}, false, shortIntType);
    proc->overload({{intType, false}}, false, intType);
    proc->overload({{longIntType, false}}, false, longIntType);
    proc->overload({{realType, false}}, false, realType);
    proc->overload({{longRealType, false}}, false, longRealType);
    // MAX
    this->createProcedure(ProcKind::MAX, "MAX", {{typeType, false}}, typeType, false, true);
    // MIN
    this->createProcedure(ProcKind::MIN, "MIN", {{typeType, false}}, typeType, false, true);

    createNamespace("SYSTEM");
    this->createProcedure(ProcKind::SYSTEM_ADR, "ADR", {{anyType, true}}, longIntType, false, true);
    this->createProcedure(ProcKind::SYSTEM_GET, "GET", {{longIntType, false}, {anyType, true}}, nullptr, false, true);
    this->createProcedure(ProcKind::SYSTEM_PUT, "PUT", {{longIntType, false}, {anyType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::SYSTEM_BIT, "BIT", {{longIntType, false}, {intType, false}}, boolType, false, true);
    this->createProcedure(ProcKind::SYSTEM_COPY, "COPY", {{longIntType, false}, {longIntType, false}, {longIntType, false}}, nullptr, false, true);
    this->createProcedure(ProcKind::SYSTEM_SIZE, "SIZE", {{typeType, false}}, longIntType, false, true);
    this->createProcedure(ProcKind::SYSTEM_VAL, "VAL", {{typeType, false}, {anyType, false}}, typeType, false, true);
    leaveNamespace();
}
