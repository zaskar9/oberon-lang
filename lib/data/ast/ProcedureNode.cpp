/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include "ProcedureNode.h"
#include "NodeVisitor.h"

ProcedureTypeNode *ProcedureNode::getType() const {
    return dynamic_cast<ProcedureTypeNode*>(DeclarationNode::getType());
}

void ProcedureNode::setExtern(bool value) {
    extern_ = value;
}

bool ProcedureNode::isExtern() const {
    return extern_;
}

bool ProcedureNode::isImported() const {
    return imported_;
}

void ProcedureNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureNode::print(std::ostream &stream) const {
    stream << "PROCEDURE " << *getIdentifier() << ";";
}