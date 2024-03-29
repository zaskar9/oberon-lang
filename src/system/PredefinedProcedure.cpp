//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "PredefinedProcedure.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

using std::make_unique;
using std::pair;
using std::string;
using std::vector;

PredefinedProcedure::PredefinedProcedure(ProcKind kind, const string &name,
                                         const vector<pair<TypeNode *, bool>> &pairs, bool varargs, TypeNode *ret) :
        ProcedureNode(make_unique<IdentDef>(name), nullptr), types_(), isCast_(), kind_(kind) {
    auto type = overload(pairs, varargs, ret);
    this->setType(type);
    isCast_ = false;
    if (ret && ret->kind() == TypeKind::TYPE) {
        for (auto p : pairs) {
            if (p.first->kind() == TypeKind::TYPE) {
                if (isCast_) {
                    isCast_ = false; // only one type parameter expected
                    break;
                }
                isCast_ = true;
            }
        }
    }
}

PredefinedProcedure::~PredefinedProcedure() = default;

ProcedureTypeNode *
PredefinedProcedure::overload(const vector<pair<TypeNode *, bool>> &pairs, bool varargs, TypeNode *ret) {
    vector<unique_ptr<ParameterNode>> params;
    params.reserve(pairs.size());
    for (auto p : pairs) {
        params.push_back(make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>("_"), p.first, p.second));
    }
    types_.push_back(make_unique<ProcedureTypeNode>(EMPTY_POS, std::move(params), varargs, ret));
    return types_.back().get();
}

ProcedureTypeNode *PredefinedProcedure::dispatch(vector<TypeNode *> actuals, TypeNode *typeType) {
    if (isCast_ && typeType) {
        auto signature = types_[0].get();
        signature->setReturnType(typeType); // TODO : mutate not ideal
        return signature;
    }
    for (const auto& type : types_) {
        auto signature = type.get();
        if (actuals.size() >= signature->parameters().size()) {
            bool match = true;
            for (size_t i = 0; i < signature->parameters().size(); i++) {
                match = match && (actuals[i] == signature->parameters()[i]->getType());
            }
            if (match) {
                return signature;
            }
        }
    }
    return nullptr;
}

bool PredefinedProcedure::isOverloaded() const {
    return (types_.size() > 1) || isCast_;
}

ProcKind
PredefinedProcedure::getKind() const {
    return kind_;
}