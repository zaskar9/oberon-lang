//
// Created by Michael Grossniklaus on 3/17/22.
//

#include "ProcedureTypeNode.h"
#include "NodeVisitor.h"
#include <algorithm>

void ProcedureTypeNode::addFormalParameter(std::unique_ptr<ParameterNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

ParameterNode *ProcedureTypeNode::getFormalParameter(const std::string &name) {
    auto result = std::find_if(parameters_.begin(), parameters_.end(),
                               [&](std::unique_ptr<ParameterNode> &param) { return param->getIdentifier()->name() == name; });
    if (result != parameters_.end()) {
        return (*result).get();
    }
    return nullptr;
}

ParameterNode *ProcedureTypeNode::getFormalParameter(size_t num) const {
    return parameters_[num].get();
}

size_t ProcedureTypeNode::getFormalParameterCount() const {
    return parameters_.size();
}

void ProcedureTypeNode::setVarArgs(bool value) {
    varargs_ = value;
}

bool ProcedureTypeNode::hasVarArgs() const {
    return varargs_;
}

void ProcedureTypeNode::setReturnType(TypeNode *type) {
    type_ = type;
}

TypeNode *ProcedureTypeNode::getReturnType() const {
    return type_;
}

void ProcedureTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureTypeNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << getIdentifier() << ";";
}

