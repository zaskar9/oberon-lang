/*
 * AST nodes representing declarations in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "DeclarationNode.h"
#include "NodeVisitor.h"

std::string DeclarationNode::getName() const {
    return name_;
}

void DeclarationNode::setType(TypeNode *type) {
    type_ = type;
}

TypeNode* DeclarationNode::getType() const {
    return type_;
}

void DeclarationNode::print(std::ostream &stream) const {
    stream << name_ << ": " << *type_;
}

void DeclarationNode::setLevel(unsigned int level) {
    level_ = level;
}

unsigned int DeclarationNode::getLevel() const {
    return level_;
}


void ConstantDeclarationNode::setValue(std::unique_ptr<ExpressionNode> value) {
    value_ = std::move(value);
}

ExpressionNode * ConstantDeclarationNode::getValue() const {
    return value_.get();
}

void ConstantDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ConstantDeclarationNode::print(std::ostream &stream) const {
    stream << "CONST " << getName() << " = " << *value_ << ";";
}


void TypeDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void TypeDeclarationNode::print(std::ostream& stream) const {
    stream << "TYPE " << getName() << " = " << *getType() << ";";
}


void VariableDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}


void FieldNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}


bool ParameterNode::isVar() const {
    return var_;
}

void ParameterNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ParameterNode::print(std::ostream &stream) const {
    stream << (var_ ? "VAR " : "") << getName() << ": " << *getType();
}