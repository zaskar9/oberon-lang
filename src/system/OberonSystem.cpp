//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "OberonSystem.h"

OberonSystem::~OberonSystem() = default;

SymbolTable *OberonSystem::getSymbolTable() {
    if (symbols_ == nullptr) {
        symbols_ = std::make_unique<SymbolTable>();
        this->initSymbolTable(symbols_.get());
    }
    return symbols_.get();
}

void OberonSystem::createBasicTypes(std::vector<std::pair<std::pair<TypeKind, unsigned int>, bool>> types) {
    for (auto pair: types) {
        auto type = createBasicType(pair.first.first, pair.first.second);
        if (pair.second) {
            symbols_->insertGlobal(type->getIdentifier()->name(), type);
        }
    }
}

BasicTypeNode *OberonSystem::createBasicType(TypeKind kind, unsigned int size) {
    auto type = std::make_unique<BasicTypeNode>(std::make_unique<Ident>(to_string(kind)), kind, size);
    auto ptr = type.get();
    predefines_.push_back(std::move(type));
    symbols_->setRef((char) kind, ptr);
    baseTypes_[ptr->getIdentifier()->name()] = ptr;
    return ptr;
}

BasicTypeNode *OberonSystem::getBasicType(TypeKind kind) {
    return baseTypes_[to_string(kind)];
}

PointerTypeNode *OberonSystem::createPointerType(TypeNode *base) {
    auto type = std::make_unique<PointerTypeNode>(EMPTY_POS, nullptr, base);
    auto ptr = type.get();
    predefines_.push_back(std::move(type));
    symbols_->setRef((char) TypeKind::POINTER, ptr);
    return ptr;
}

void OberonSystem::createProcedure(ProcType type, std::string name, std::vector<std::pair<TypeNode *, bool>> params,
                                   TypeNode *ret, bool hasVarArgs, bool toSymbols) {
    auto proc = std::make_unique<PredefinedProcedure>(type, name, params, ret);
    proc->setVarArgs(hasVarArgs);
    auto ptr = proc.get();
    predefines_.push_back(std::move(proc));
    if (toSymbols) {
        symbols_->insertGlobal(ptr->getIdentifier()->name(), ptr);
    }
}

void Oberon07::initSymbolTable(SymbolTable *symbols) {
    this->createBasicTypes(
            {
                    {{TypeKind::ANYTYPE,  0}, false},
                    {{TypeKind::NOTYPE,   0}, false},
                    {{TypeKind::NILTYPE,  8}, false},
                    {{TypeKind::BOOLEAN,  1}, true},
                    {{TypeKind::BYTE,     1}, true},
                    {{TypeKind::CHAR,     1}, true},
                    {{TypeKind::INTEGER,  4}, true},
                    {{TypeKind::LONGINT,  8}, true},
                    {{TypeKind::REAL,     4}, true},
                    {{TypeKind::LONGREAL, 8}, true},
                    {{TypeKind::STRING,   8}, true}
            }
    );

    symbols->setNilType(this->getBasicType(TypeKind::NILTYPE));
    auto anyType = this->getBasicType(TypeKind::ANYTYPE);
    auto boolType = this->getBasicType(TypeKind::BOOLEAN);
    auto intType = this->getBasicType(TypeKind::INTEGER);
    auto longType = this->getBasicType(TypeKind::LONGINT);

    this->createProcedure(ProcType::NEW, "NEW", {{this->createPointerType(anyType), true}}, nullptr, false, true);
    this->createProcedure(ProcType::FREE, "FREE", {{this->createPointerType(anyType), true}}, nullptr, false, true);
    this->createProcedure(ProcType::INC, "INC", {{longType, true}}, nullptr, true, true);
    this->createProcedure(ProcType::DEC, "DEC", {{longType, true}}, nullptr, true, true);
    this->createProcedure(ProcType::LSL, "LSL", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcType::ASR, "ASR", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcType::ROL, "ROL", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcType::ROR, "ROR", {{longType, false}, {longType, false}}, longType, false, true);
    this->createProcedure(ProcType::ODD, "ODD", {{longType, false}}, boolType, false, true);
    this->createProcedure(ProcType::HALT, "HALT", {{ intType, false}}, nullptr, false, true);
    this->createProcedure(ProcType::ASSERT, "ASSERT", {{boolType, false}}, nullptr, false, true);
}
