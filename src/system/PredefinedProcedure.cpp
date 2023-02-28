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


const std::string Lsl::NAME = "LSL";

void Lsl::setup(OberonSystem *system) {
    auto longType = system->getBasicType(TypeKind::LONGINT);
    this->setSignature({ { longType, false }, { longType, false } }, longType);
}


const std::string Asr::NAME = "ASR";

void Asr::setup(OberonSystem *system) {
    auto longType = system->getBasicType(TypeKind::LONGINT);
    this->setSignature({ { longType, false }, { longType, false } }, longType);
}


const std::string Ror::NAME = "ROR";

void Ror::setup(OberonSystem *system) {
    auto longType = system->getBasicType(TypeKind::LONGINT);
    this->setSignature({ { longType, false }, { longType, false } }, longType);
}


const std::string Rol::NAME = "ROL";

void Rol::setup(OberonSystem *system) {
    auto longType = system->getBasicType(TypeKind::LONGINT);
    this->setSignature({ { longType, false }, { longType, false } }, longType);
}


const std::string Odd::NAME = "ODD";

void Odd::setup(OberonSystem *system) {
    auto longType = system->getBasicType(TypeKind::LONGINT);
    auto boolType = system->getBasicType(TypeKind::BOOLEAN);
    this->setSignature({ { longType, false } }, boolType);
}


const std::string Halt::NAME = "HALT";

void Halt::setup(OberonSystem *system) {
    auto intType = system->getBasicType(TypeKind::INTEGER);
    this->setSignature({ { intType, false} }, nullptr);
}


const std::string Assert::NAME = "ASSERT";

void Assert::setup(OberonSystem *system) {
    auto boolType = system->getBasicType(TypeKind::BOOLEAN);
    this->setSignature({ { boolType, false } }, nullptr);
}