/*
 * AST node representing a code block in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "BlockNode.h"
#include "ProcedureNode.h"

BlockNode::BlockNode(vector<unique_ptr<ConstantDeclarationNode>> consts,
                     vector<unique_ptr<TypeDeclarationNode>> types,
                     vector<unique_ptr<VariableDeclarationNode>> vars,
                     vector<unique_ptr<ProcedureNode>> procs,
                     unique_ptr<StatementSequenceNode> stmts) :
        constants_(std::move(consts)), type_declarations_(std::move(types)), variables_(std::move(vars)),
        procedures_(std::move(procs)), statements_(std::move(stmts)) { }

BlockNode::BlockNode() : constants_(), type_declarations_(), variables_(), procedures_(),
        statements_(make_unique<StatementSequenceNode>(EMPTY_POS)) { }

BlockNode::~BlockNode() = default;

vector<unique_ptr<ConstantDeclarationNode>> &BlockNode::constants() {
    return constants_;
}

void BlockNode::addConstant(std::unique_ptr<ConstantDeclarationNode> constant) {
    constants_.push_back(std::move(constant));
}

ConstantDeclarationNode* BlockNode::getConstant(size_t num) const {
    return constants_.at(num).get();
}

size_t BlockNode::getConstantCount() const {
    return constants_.size();
}

vector<unique_ptr<TypeDeclarationNode>> &BlockNode::types() {
    return type_declarations_;
}

void BlockNode::addTypeDeclaration(std::unique_ptr<TypeDeclarationNode> type_declaration) {
    type_declarations_.push_back(std::move(type_declaration));
}

TypeDeclarationNode* BlockNode::getTypeDeclaration(size_t num) const {
    return type_declarations_.at(num).get();
}

size_t BlockNode::getTypeDeclarationCount() const {
    return type_declarations_.size();
}

vector<unique_ptr<VariableDeclarationNode>> &BlockNode::variables() {
    return variables_;
}

void BlockNode::addVariable(std::unique_ptr<VariableDeclarationNode> variable) {
    variables_.push_back(std::move(variable));
}

void BlockNode::insertVariable(size_t pos, std::unique_ptr<VariableDeclarationNode> variable) {
    variables_.insert(variables_.begin() + (long) pos, std::move(variable));
}

VariableDeclarationNode* BlockNode::getVariable(size_t num) const {
    return variables_.at(num).get();
}

size_t BlockNode::getVariableCount() const {
    return variables_.size();
}

void BlockNode::removeVariables(size_t from, size_t to) {
    variables_.erase(variables_.begin() + (long) from, variables_.begin() + (long) to);
}

vector<unique_ptr<ProcedureNode>> &BlockNode::procedures() {
    return procedures_;
}

void BlockNode::addProcedure(std::unique_ptr<ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

ProcedureNode* BlockNode::getProcedure(size_t num) const {
    return procedures_.at(num).get();
}

size_t BlockNode::getProcedureCount() const {
    return procedures_.size();
}

std::unique_ptr<ProcedureNode> BlockNode::removeProcedure(size_t num) {
    auto res = std::move(procedures_[num]);
    procedures_.erase(procedures_.begin() + (long) num);
    return res;
}

StatementSequenceNode* BlockNode::statements() {
    return statements_.get();
}
