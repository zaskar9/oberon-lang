//
// Created by Michael Grossniklaus on 1/28/24.
//

#include "ASTContext.h"
#include <memory>

using std::make_unique;

ArrayTypeNode *ASTContext::getOrInsertArrayType(unsigned int dimension, TypeNode *memberType) {
    for (auto &type : array_ts_) {
        if (type->getMemberType() == memberType && type->getDimension() == dimension) {
            return type.get();
        }
    }
    auto type = make_unique<ArrayTypeNode>(EMPTY_POS, nullptr, dimension, memberType);
    auto res = type.get();
    array_ts_.push_back(std::move(type));
    return res;
}

RecordTypeNode *ASTContext::getOrInsertRecordType([[maybe_unused]] vector<unique_ptr<FieldNode>> fields) {
    return nullptr;
}

PointerTypeNode *ASTContext::getOrInsertPointerType([[maybe_unused]] TypeNode *memberType) {
    return nullptr;
}

ProcedureTypeNode *ASTContext::getOrInsertProcedureNode([[maybe_unused]] vector<unique_ptr<ParameterNode>> params,
                                                        [[maybe_unused]] TypeNode *return_t) {
    return nullptr;
}

TypeReferenceNode *ASTContext::getOrInsertTypeReference([[maybe_unused]] unique_ptr<QualIdent> ident) {
    return nullptr;
}