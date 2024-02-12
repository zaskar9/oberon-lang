/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include <algorithm>
#include "ProcedureNode.h"
#include "NodeVisitor.h"

ProcedureNode::ProcedureNode(const FilePos &pos, std::unique_ptr<IdentDef> ident, bool external) :
        DeclarationNode(NodeType::procedure, pos, std::move(ident), nullptr),
        BlockNode(),
        extern_(external) {}

ProcedureNode::ProcedureNode(unique_ptr<IdentDef> ident, ProcedureTypeNode *type, bool external) :
        DeclarationNode(NodeType::procedure, EMPTY_POS, std::move(ident), type),
        BlockNode(),
        extern_(external) {}

ProcedureTypeNode *ProcedureNode::proctype() const {
    return dynamic_cast<ProcedureTypeNode*>(this->getType());
}

void ProcedureNode::addFormalParameter(std::unique_ptr<ParameterNode> parameter) {
    return proctype()->addFormalParameter(std::move(parameter));
}

ParameterNode *ProcedureNode::getFormalParameter(const std::string &name) {
    return proctype()->getFormalParameter(name);
}

ParameterNode *ProcedureNode::getFormalParameter(size_t num) const {
    return proctype()->getFormalParameter(num);
}

size_t ProcedureNode::getFormalParameterCount() const {
    return proctype()->getFormalParameterCount();
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
    stream << "PROCEDURE " << *getIdentifier() << ";";
}