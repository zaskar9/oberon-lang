/*
 * Implementation of the AST procedure call node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "CallNode.h"
#include "NodeVisitor.h"


void CallNode::addParameter(std::unique_ptr<ExpressionNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

ExpressionNode * CallNode::getParameter(size_t num) const {
    return parameters_.at(num).get();
}

size_t CallNode::getParameterCount() const {
    return parameters_.size();
}

ProcedureNode * CallNode::getProcedure() const {
    return procedure_;
}

bool FunctionCallNode::isConstant() const {
    return false;
}

TypeNode * FunctionCallNode::getType() const {
    return this->getProcedure()->getReturnType();
}

void FunctionCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void FunctionCallNode::print(std::ostream &stream) const {
    stream << this->getProcedure()->getName() << "()";
}

void ProcedureCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureCallNode::print(std::ostream &stream) const {
    stream << this->getProcedure()->getName() << "()";
}
