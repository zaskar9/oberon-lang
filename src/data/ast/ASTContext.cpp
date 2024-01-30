//
// Created by Michael Grossniklaus on 1/28/24.
//

#include "ASTContext.h"
#include <memory>

using std::make_unique;

Node *ASTContext::getTranslationUnit() {
    return unit_.get();
}

void ASTContext::setTranslationUnit(unique_ptr<Node> unit) {
    unit_ = std::move(unit);
}

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

ProcedureTypeNode *ASTContext::getOrInsertProcedureNode(vector<unique_ptr<ParameterNode>> params,
                                                        TypeNode *returnType) {
    auto type = make_unique<ProcedureTypeNode>(EMPTY_POS, std::move(params), returnType);
    auto res = type.get();
    procedure_ts.push_back(std::move(type));
    return res;
}

TypeReferenceNode *ASTContext::getOrInsertTypeReference(unique_ptr<QualIdent> ident) {
    auto ref = make_unique<TypeReferenceNode>(EMPTY_POS, std::move(ident));
    auto res = ref.get();
    references_.push_back(std::move(ref));
    return res;
}