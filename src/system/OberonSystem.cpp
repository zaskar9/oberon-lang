//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "OberonSystem.h"
#include "PredefinedProcedure.h"

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

void OberonSystem::createProcedure(std::unique_ptr<ProcedureNode> proc, bool toSymbols) {
    auto ptr = proc.get();
    predefines_.push_back(std::move(proc));
    if (ptr->isPredefined()) {
        auto predef = dynamic_cast<PredefinedProcedure *>(ptr);
        predef->setup(this);
    }
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
    this->createProcedure(std::make_unique<New>(), true);
}
