/*
 * AST nodes representing declarations in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "DeclarationNode.h"
#include "NodeVisitor.h"

void DeclarationNode::setModule(ModuleNode *module) {
    module_ = module;
}

ModuleNode *DeclarationNode::getModule() const {
    return module_;
}

void DeclarationNode::setIdentifier(std::unique_ptr<IdentDef> name) {
    ident_ = std::move(name);
}

IdentDef* DeclarationNode::getIdentifier() const {
    return ident_.get();
}

void DeclarationNode::setType(TypeNode *type) {
    type_ = type;
}

TypeNode* DeclarationNode::getType() const {
    return type_;
}

void DeclarationNode::print(std::ostream &stream) const {
    stream << *getIdentifier() << ": " << *getType();
}

unsigned int DeclarationNode::seqId() const {
    return seqId_;
}

void DeclarationNode::setScope(unsigned int scope) {
    scope_ = scope;
}

unsigned int DeclarationNode::getScope() const {
    return scope_;
}


ExpressionNode * ConstantDeclarationNode::getValue() const {
    return value_.get();
}

void ConstantDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ConstantDeclarationNode::print(std::ostream &stream) const {
    stream << "CONST " << *getIdentifier() << " = " << *value_ << ";";
}


TypeDeclarationNode::TypeDeclarationNode(const FilePos &pos, unique_ptr<IdentDef> ident, TypeNode *type) :
        DeclarationNode(NodeType::type, pos, std::move(ident), type, 0) {
   if (type->isAnonymous()) {
       type->setDeclaration(this);
   }
}

void TypeDeclarationNode::setModule(ModuleNode *module) {
    DeclarationNode::setModule(module);
    this->getType()->setModule(module);
}

void TypeDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void TypeDeclarationNode::print(std::ostream& stream) const {
    stream << "TYPE " << *getIdentifier() << " = " << *getType() << ";";
}


void VariableDeclarationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void FieldNode::setRecordType(RecordTypeNode *parent) {
    parent_ = parent;
}

void FieldNode::setIndex(const unsigned index) {
    index_ = index;
}

RecordTypeNode *FieldNode::getRecordType() const {
    return parent_;
}

unsigned int FieldNode::getIndex() const {
    return index_;
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
    stream << (var_ ? "VAR " : "") << *getIdentifier() << ": " << *getType();
}