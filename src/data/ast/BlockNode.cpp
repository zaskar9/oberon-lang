/*
 * AST node representing a code block in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "BlockNode.h"

void BlockNode::registerType(std::unique_ptr<TypeNode> type) {
    types_.push_back(std::move(type));
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

void BlockNode::addTypeDeclaration(std::unique_ptr<TypeDeclarationNode> type_declaration) {
    type_declarations_.push_back(std::move(type_declaration));
}

TypeDeclarationNode* BlockNode::getTypeDeclaration(size_t num) const {
    return type_declarations_.at(num).get();
}

size_t BlockNode::getTypeDeclarationCount() const {
    return type_declarations_.size();
}

void BlockNode::addVariable(std::unique_ptr<VariableDeclarationNode> variable) {
    variables_.push_back(std::move(variable));
}

VariableDeclarationNode* BlockNode::getVariable(size_t num) const {
    return variables_.at(num).get();
}

size_t BlockNode::getVariableCount() const {
    return variables_.size();
}

StatementSequenceNode* BlockNode::getStatements() {
    return statements_.get();
}
