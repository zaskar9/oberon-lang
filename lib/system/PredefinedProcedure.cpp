//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "PredefinedProcedure.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

using std::make_unique;
using std::pair;
using std::string;
using std::unordered_set;
using std::vector;

PredefinedProcedure::PredefinedProcedure(const ProcKind kind, const string &name,
                                         const vector<pair<TypeNode *, bool>> &pairs, const bool varargs, TypeNode *ret) :
        ProcedureNode(EMPTY_POS, make_unique<IdentDef>(name), nullptr, CallingConvention::OLANG),
        isCast_(), kind_(kind) {
    const auto type = overload(pairs, varargs, ret);
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

ProcedureTypeNode *PredefinedProcedure::dispatch(const vector<TypeNode *> &actuals, TypeNode *typeType) const {
    if (isCast_ && typeType) {
        const auto signature = types_[0].get();
        signature->setReturnType(typeType); // TODO avoid mutation of type node
        return signature;
    }
    int max_score = 0;
    unordered_set<ProcedureTypeNode *> winners;
    for (const auto& type : types_) {
        auto signature = type.get();
        if (actuals.size() >= signature->parameters().size()) {
            int score = 0;
            for (size_t i = 0; i < signature->parameters().size(); i++) {
                const int match = matchType(signature->parameters()[i]->getType(), actuals[i]);
                if (match == 0) {
                    score = -1;
                    break;
                }
                score += match;
            }
            if (score > max_score) {
                winners.clear();
                winners.insert(signature);
                max_score = score;
            } else if (score == max_score) {
                winners.insert(signature);
            }
        }
    }
    if (winners.size() == 1) {
        return *winners.begin();
    }
    return nullptr;
}

int PredefinedProcedure::matchType(const TypeNode *expected, const TypeNode *actual) {
    if (expected == actual) {
        return 2;
    }
    if ((expected->kind() == TypeKind::ENTIRE && actual->isInteger()) ||
        (expected->kind() == TypeKind::FLOATING && actual->isReal()) ||
        (expected->kind() == TypeKind::NUMERIC && actual->isNumeric()) ||
        expected->kind() == TypeKind::ANYTYPE) {
        return 1;
    }
    return 0;
}

bool PredefinedProcedure::isOverloaded() const {
    return (types_.size() > 1) || isCast_;
}

ProcKind
PredefinedProcedure::getKind() const {
    return kind_;
}