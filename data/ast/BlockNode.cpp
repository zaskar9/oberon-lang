/*
 * AST node representing a code block in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "BlockNode.h"

BlockNode::BlockNode(const NodeType nodeType, const FilePos &pos, int level) : Node(nodeType, pos), level_(level),
        types_(), constants_(), type_declarations_(), variables_(),
        statements_(std::make_unique<StatementSequenceNode>(pos)), return_() {
}

BlockNode::~BlockNode() = default;

int BlockNode::getLevel() const {
    return level_;
}

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

void BlockNode::setReturnType(TypeNode *type) {
    return_ = type;
}

TypeNode * BlockNode::getReturnType() const {
    return return_;
}


void BlockNode::print(std::ostream& stream) const {
    for (auto const& constant: constants_) {
        stream << *constant << std::endl;
    }
    for (auto const& type: types_) {
        stream << *type << std::endl;
    }
    for (auto const& variable: variables_) {
        stream << *variable << std::endl;
    }
    stream << "BEGIN" << std::endl << *statements_;
}