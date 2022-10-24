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
    this->setSignature({{ system->createPointerType(anyType), true }}, nullptr);
}

Value *New::call(IRBuilder<> *builder, Module *module, std::vector<Value *> params) {
    auto type = FunctionType::get(builder->getInt8PtrTy(), {builder->getInt64Ty()}, false);
    auto callee = module->getOrInsertFunction("malloc", type);
    std::vector<Value *> values;
    auto pointer_t = (PointerType *) params[0]->getType();
    auto layout = module->getDataLayout();
    auto base = pointer_t->getContainedType(0);
    values.push_back(ConstantInt::get(builder->getInt64Ty(), layout.getTypeAllocSize(base->getContainedType(0))));
    Value *value = builder->CreateCall(callee, values);
    value = builder->CreateBitCast(value, base);
    return builder->CreateStore(value, params[0]);
}