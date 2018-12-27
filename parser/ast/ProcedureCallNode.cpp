/*
 * Implementation of the AST procedure call node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "ProcedureCallNode.h"

ProcedureCallNode::ProcedureCallNode(const FilePos pos, const ProcedureNode* procedure) :
        StatementNode(NodeType::procedure_call, pos), procedure_(procedure) {
}

ProcedureCallNode::~ProcedureCallNode() = default;

void ProcedureCallNode::print(std::ostream& stream) const {
    stream << procedure_->getName() << "()";
}
