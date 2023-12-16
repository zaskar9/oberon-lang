//
// Created by Michael Grossniklaus on 10/23/22.
//

#include "PredefinedProcedure.h"

PredefinedProcedure::PredefinedProcedure(ProcType type, std::string name, std::vector<std::pair<TypeNode *, bool>> params,
                                         TypeNode *ret) : ProcedureNode(EMPTY_POS, std::make_unique<Ident>(name)), type_(type) {
    for (auto p: params) {
        auto param = std::make_unique<ParameterNode>(EMPTY_POS, std::make_unique<Ident>("_"), p.first, p.second);
        this->addFormalParameter(std::move(param));
    }
    this->setReturnType(ret);
}

PredefinedProcedure::~PredefinedProcedure() = default;

ProcType PredefinedProcedure::getProcType() const {
    return type_;
}