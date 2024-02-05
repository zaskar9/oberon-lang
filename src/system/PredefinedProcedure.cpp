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
                                         const vector<pair<TypeNode *, bool>> &params, TypeNode *ret) :
        ProcedureNode(make_unique<Ident>(name), nullptr), kind_(kind) {
    type_ = make_unique<ProcedureTypeNode>(EMPTY_POS, this->getIdentifier());
    this->setType(type_.get());
    for (auto p: params) {
        auto param = std::make_unique<ParameterNode>(EMPTY_POS, std::make_unique<Ident>("_"), p.first, p.second);
        this->addFormalParameter(std::move(param));
    }
    this->setReturnType(ret);
}

PredefinedProcedure::~PredefinedProcedure() = default;

ProcKind PredefinedProcedure::getKind() const {
    return kind_;
}