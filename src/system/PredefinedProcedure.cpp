//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "PredefinedProcedure.h"

PredefinedProcedure::~PredefinedProcedure() = default;

void PredefinedProcedure::setSignature(std::vector<std::pair<TypeNode *, bool>> params, TypeNode *ret) {
    for (auto p: params) {
        auto param = std::make_unique<ParameterNode>(EMPTY_POS, std::make_unique<Ident>("_"), p.first, p.second);
        this->addFormalParameter(std::move(param));
    }
    this->setReturnType(ret);
}

const std::string New::NAME = "NEW";

void New::setup(OberonSystem *system) {
    auto anyType = system->getBasicType(TypeKind::ANYTYPE);
    this->setSignature({ { system->createPointerType(anyType), true } }, nullptr);
}

const std::string Free::NAME = "FREE";

void Free::setup(OberonSystem *system) {
    auto anyType = system->getBasicType(TypeKind::ANYTYPE);
    this->setSignature({ { system->createPointerType(anyType), true } }, nullptr);
}

const std::string Inc::NAME = "INC";

void Inc::setup(OberonSystem *system) {
    auto longType = system->getBasicType(TypeKind::LONGINT);
    this->setSignature({ { longType, true } }, nullptr);
    this->setVarArgs(true);
}

const std::string Dec::NAME = "DEC";

void Dec::setup(OberonSystem *system) {
    auto longType = system->getBasicType(TypeKind::LONGINT);
    this->setSignature({ { longType, true } }, nullptr);
    this->setVarArgs(true);
}