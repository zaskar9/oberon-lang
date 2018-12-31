/*
 * Implementation of the AST procedure call node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "ProcedureCallNode.h"
#include "NodeVisitor.h"

ProcedureCallNode::ProcedureCallNode(const FilePos pos, ProcedureNode* procedure) :
        StatementNode(NodeType::procedure_call, pos), procedure_(procedure), parameters_() {
}

ProcedureCallNode::~ProcedureCallNode() = default;

void ProcedureCallNode::addParameter(std::unique_ptr<ExpressionNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

ExpressionNode* ProcedureCallNode::getParameter(size_t num) const {
    return parameters_.at(num).get();
}

size_t ProcedureCallNode::getParameterCount() const {
    return parameters_.size();
}

ProcedureNode* ProcedureCallNode::getProcedure() {
    return procedure_;
}

void ProcedureCallNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ProcedureCallNode::print(std::ostream& stream) const {
    stream << procedure_->getName() << "()";
}
