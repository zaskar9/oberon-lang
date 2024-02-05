/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#include <algorithm>
#include "ProcedureNode.h"
#include "NodeVisitor.h"

ProcedureNode::ProcedureNode(const FilePos &pos, std::unique_ptr<Ident> ident,
                             ProcedureTypeNode *type,
                             vector<unique_ptr<ConstantDeclarationNode>> consts,
                             vector<unique_ptr<TypeDeclarationNode>> types,
                             vector<unique_ptr<VariableDeclarationNode>> vars,
                             vector<unique_ptr<ProcedureNode>> procs,
                             unique_ptr<StatementSequenceNode> stmts) :
        DeclarationNode(NodeType::procedure, pos, std::move(ident), type),
        BlockNode(std::move(consts), std::move(types), std::move(vars), std::move(procs), std::move(stmts)),
        extern_(false) {}

ProcedureNode::ProcedureNode(unique_ptr<Ident> ident, ProcedureTypeNode *type, bool external) :
        DeclarationNode(NodeType::procedure_type, EMPTY_POS, std::move(ident), type),
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