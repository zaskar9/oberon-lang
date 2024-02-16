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
        ProcedureNode(make_unique<IdentDef>(name), nullptr), kind_(kind) {
    vector<unique_ptr<ParameterNode>> params;
    params.reserve(pairs.size());
    for (auto p : pairs) {
        params.push_back(std::make_unique<ParameterNode>(EMPTY_POS, make_unique<Ident>("_"), p.first, p.second));
    }
    type_ = make_unique<ProcedureTypeNode>(this->getIdentifier(), std::move(params), varargs, ret);
    this->setType(type_.get());
}

PredefinedProcedure::~PredefinedProcedure() = default;

ProcKind PredefinedProcedure::getKind() const {
    return kind_;
}