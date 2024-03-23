//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "PredefinedProcedure.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

using std::make_unique;
using std::pair;
using std::string;
using std::vector;
using std::cout;

PredefinedProcedure::PredefinedProcedure(ProcKind kind, const string &name,
                                         const vector<pair<TypeNode *, bool>> &pairs, bool varargs, TypeNode *ret) :
        ProcedureNode(make_unique<IdentDef>(name), nullptr), types_(), castIdx_(), kind_(kind) {
    auto type = overload(pairs, varargs, ret);
    this->setType(type);
    castIdx_ = 0;
    if (ret != nullptr) {
        if (ret->kind() == TypeKind::TYPE) {
            castIdx_ = -1;
            int i = 1;
            for (auto p : pairs) {
                if (p.first->kind() == TypeKind::TYPE) {
                    if (castIdx_ != -1) {
                        castIdx_ = -1;
                        break;
                    }
                    castIdx_ = i;
                }
                i++;
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
        params.push_back(std::make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>("_"), p.first, p.second));
    }
    types_.push_back(make_unique<ProcedureTypeNode>(EMPTY_POS, this->getIdentifier(), std::move(params), varargs, ret));
    return types_.back().get();
}

ProcedureTypeNode *PredefinedProcedure::dispatch(vector<TypeNode *> actuals, TypeNode *typeType) const {
    if (castIdx_ < 0) {
        return nullptr;
    }
    if ((castIdx_ > 0) && typeType) {
        auto signature = make_unique<ProcedureTypeNode>(EMPTY_POS, this->getIdentifier(), std::move(types_[0]->parameters()), types_[0]->hasVarArgs(), typeType);
        return std::move(signature.get());
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
    return (types_.size() > 1) || (castIdx_ != 0);
}

ProcKind
PredefinedProcedure::getKind() const {
    return kind_;
}