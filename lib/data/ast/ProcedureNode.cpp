/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ProcedureNode.h"
#include "NodeVisitor.h"

ProcedureNode::~ProcedureNode() = default;

CallingConvention ProcedureNode::getConvention() const {
    return conv_;
}

bool ProcedureNode::isExternal() const {
    return false;
}

bool ProcedureNode::isPredefined() const {
    return false;
}

ProcedureTypeNode *ProcedureNode::getType() const {
    return dynamic_cast<ProcedureTypeNode*>(DeclarationNode::getType());
}

void ProcedureNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << *getIdentifier() << ";";
}


string ProcedureDeclarationNode::getName() const {
    return name_;
}

bool ProcedureDeclarationNode::isExternal() const {
    return true;
}

void ProcedureDeclarationNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}


void ProcedureDefinitionNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}
