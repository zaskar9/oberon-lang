/*
 * AST nodes representing declarations in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "DeclarationNode.h"
#include "NodeVisitor.h"

const Node * DeclarationNode::getParent() const {
    return parent_;
}

const std::string DeclarationNode::getName() const {
    return name_;
}

TypeNode* DeclarationNode::getType() const {
    return type_;
}

void DeclarationNode::print(std::ostream &stream) const {
    stream << name_ << ": " << *type_;
}

int DeclarationNode::getLevel() const {
    return level_;
}


LiteralNode* ConstantDeclarationNode::getValue() const {
    return value_.get();
}

void ConstantDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ConstantDeclarationNode::print(std::ostream &stream) const {
    stream << "CONST " << this->getName() << " = " << *value_ << ";";
}


void TypeDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void TypeDeclarationNode::print(std::ostream& stream) const {
    stream << "TYPE " << this->getName() << " = " << *this->getType() << ";";
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
    stream << (var_ ? "VAR " : "") << this->getName() << ": " << *this->getType();
}