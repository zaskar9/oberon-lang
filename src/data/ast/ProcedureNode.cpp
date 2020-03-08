/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ProcedureNode.h"
#include "NodeVisitor.h"

void ProcedureNode::addParameter(std::unique_ptr<ParameterNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

ParameterNode* ProcedureNode::getParameter(const size_t num) const {
    return parameters_.at(num).get();
}

size_t ProcedureNode::getParameterCount() const {
    return parameters_.size();
}

void ProcedureNode::addProcedure(std::unique_ptr<ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

ProcedureNode* ProcedureNode::getProcedure(size_t num) const {
    return procedures_.at(num).get();
}

size_t ProcedureNode::getProcedureCount() const {
    return procedures_.size();
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

TypeNode * ProcedureNode::getReturnType() const {
    return getType();
}

void ProcedureNode::setExtern(bool value) {
    extern_ = value;
}

bool ProcedureNode::isExtern() const {
    return extern_;
}

void ProcedureNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ProcedureNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << getName() << ";";
}