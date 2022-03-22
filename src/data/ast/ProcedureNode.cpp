/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include <algorithm>
#include "ProcedureNode.h"
#include "NodeVisitor.h"

ProcedureNode::ProcedureNode(const FilePos &pos, std::unique_ptr<Identifier> ident) :
        DeclarationNode(NodeType::procedure, pos, std::move(ident), nullptr), BlockNode(pos),
        extern_(false) {
    proctype_ = std::make_unique<ProcedureTypeNode>(pos);
    this->setType(proctype_.get());
    extern_ = false;
}

ProcedureTypeNode *ProcedureNode::proctype() const {
    return dynamic_cast<ProcedureTypeNode*>(this->getType());
}

void ProcedureNode::addFormalParameter(std::unique_ptr<ParameterNode> parameter) {
    return proctype()->addParameter(std::move(parameter));
}

ParameterNode *ProcedureNode::addFormalParameter(const std::string &name) {
    return proctype()->getParameter(name);
}

ParameterNode *ProcedureNode::getFormalParameter(size_t num) const {
    return proctype()->getParameter(num);
}

size_t ProcedureNode::getFormalParameterCount() const {
    return proctype()->getParameterCount();
}

void ProcedureNode::setVarArgs(bool value) {
    proctype()->setVarArgs(value);
}

bool ProcedureNode::hasVarArgs() const {
    return proctype()->hasVarArgs();
}

void ProcedureNode::setReturnType(TypeNode *type) {
    proctype()->setReturnType(type);
}

TypeNode *ProcedureNode::getReturnType() const {
    return proctype()->getReturnType();
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
    stream << "PROCEDURE " << getIdentifier() << ";";
}