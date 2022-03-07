/*
 * AST nodes representing declarations in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "DeclarationNode.h"
#include "NodeVisitor.h"

void DeclarationNode::setIdentifier(std::unique_ptr<Identifier> name) {
    ident_ = std::move(name);
}
Identifier* DeclarationNode::getIdentifier() const {
    return ident_.get();
}

void DeclarationNode::setType(TypeNode *type) {
    type_ = type;
}

TypeNode* DeclarationNode::getType() const {
    return type_;
}

void DeclarationNode::print(std::ostream &stream) const {
    stream << this->getIdentifier()->name();
    if (this->getIdentifier()->isExported()) {
        stream << "*";
    }
    stream << ": " << this->getType();
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
    stream << "CONST " << getIdentifier() << " = " << *value_ << ";";
}


void TypeDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void TypeDeclarationNode::print(std::ostream& stream) const {
    stream << "TYPE " << getIdentifier() << " = " << *getType() << ";";
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
    stream << (var_ ? "VAR " : "") << getIdentifier() << ": " << *getType();
}