/*
 * Implementation of the AST procedure nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ProcedureNode.h"

ProcedureNode::ProcedureNode(const FilePos pos, const std::string &name) :
        BlockNode(NodeType::procedure, pos), name_(name), parameters_(), procedures_() {
}

ProcedureNode::~ProcedureNode() = default;

const std::string ProcedureNode::getName() const {
    return name_;
}

void ProcedureNode::addParameter(std::unique_ptr<const ParameterNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

const ParameterNode* ProcedureNode::getParameter(const size_t num) const {
    return parameters_.at(num).get();
}

size_t ProcedureNode::getParameterCount() const {
    return parameters_.size();
}

void ProcedureNode::addProcedure(std::unique_ptr<ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

void ProcedureNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << name_ << ";";
}