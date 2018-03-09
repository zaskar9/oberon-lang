/*
 * Implementation of the AST parameter procedure nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ProcedureNode.h"

ProcedureNode::ProcedureNode(FilePos pos, const std::string &name) :
        BlockNode(NodeType::procedure, pos), name_(name), parameters_() {
}

ProcedureNode::~ProcedureNode() = default;

const std::string ProcedureNode::getName() const {
    return name_;
}

void ProcedureNode::addParameter(std::unique_ptr<const ParameterNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

void ProcedureNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << name_;
}