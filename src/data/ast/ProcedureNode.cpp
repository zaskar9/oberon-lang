/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include <algorithm>
#include "ProcedureNode.h"
#include "NodeVisitor.h"

void ProcedureNode::addParameter(std::unique_ptr<ParameterNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

ParameterNode *ProcedureNode::getParameter(const std::string &name) {
    auto result = std::find_if(parameters_.begin(), parameters_.end(),
                               [&](std::unique_ptr<ParameterNode> &param) { return param->getName() == name; });
    if (result != parameters_.end()) {
        return (*result).get();
    }
    return nullptr;
}

ParameterNode *ProcedureNode::getParameter(const size_t num) const {
    return parameters_[num].get();
}

size_t ProcedureNode::getParameterCount() const {
    return parameters_.size();
}

void ProcedureNode::addProcedure(std::unique_ptr<ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

ProcedureNode *ProcedureNode::getProcedure(size_t num) const {
    return procedures_[num].get();
}

size_t ProcedureNode::getProcedureCount() const {
    return procedures_.size();
}

std::unique_ptr<ProcedureNode> ProcedureNode::moveProcedure(size_t num) {
    auto res = std::move(procedures_[num]);
    procedures_.erase(procedures_.begin() + (long) num);
    return res;
}

void ProcedureNode::setVarArgs(bool value) {
    varargs_ = value;
}

bool ProcedureNode::hasVarArgs() const {
    return varargs_;
}

void ProcedureNode::setReturnType(TypeNode *type) {
    setType(type);
}

TypeNode *ProcedureNode::getReturnType() const {
    return getType();
}

void ProcedureNode::setExtern(bool value) {
    extern_ = value;
}

bool ProcedureNode::isExtern() const {
    return extern_;
}

void ProcedureNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << getName() << ";";
}