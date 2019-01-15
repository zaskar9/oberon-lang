/*
 * Implementation of the AST procedure nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ProcedureNode.h"
#include "NodeVisitor.h"

const std::string ProcedureNode::getName() const {
    return name_;
}

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

void ProcedureNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ProcedureNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << name_ << ";";
}