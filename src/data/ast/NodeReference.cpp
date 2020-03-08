/*
 * AST node representing a reference to a declaration in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "NodeReference.h"
#include "NodeVisitor.h"

std::string ValueReferenceNode::getName() const {
    return name_;
}

bool ValueReferenceNode::isResolved() const {
    return (node_ != nullptr);
}

void ValueReferenceNode::resolve(DeclarationNode *node) {
    node_ = node;
}

DeclarationNode* ValueReferenceNode::dereference() const {
    return node_;
}

void ValueReferenceNode::addSelector(NodeType nodeType, std::unique_ptr<ExpressionNode> selector) {
    selectors_.push_back(std::move(selector));
    types_.push_back(nodeType);
}

ExpressionNode* ValueReferenceNode::getSelector(size_t num) const {
    return selectors_[num].get();
}

NodeType ValueReferenceNode::getSelectorType(size_t num) const {
    return types_[num];
}

size_t ValueReferenceNode::getSelectorCount() const {
    return selectors_.size();
}

bool ValueReferenceNode::isConstant() const {
    return dereference()->getNodeType() == NodeType::constant;
}

void ValueReferenceNode::setType(TypeNode *type) {
    type_ = type;
}

TypeNode* ValueReferenceNode::getType() const {
    if (type_ == nullptr) {
        return dereference()->getType();
    }
    return type_;
}

void ValueReferenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ValueReferenceNode::print(std::ostream &stream) const {
    stream << *dereference();
}


bool TypeReferenceNode::isResolved() const {
    return (node_ != nullptr);
}

void TypeReferenceNode::resolve(TypeNode *node) {
    node_ = node;
}

TypeNode * TypeReferenceNode::dereference() const {
    return node_;
}

unsigned int TypeReferenceNode::getSize() const {
    if (node_) {
        return node_->getSize();
    }
    return TypeNode::getSize();
}

void TypeReferenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void TypeReferenceNode::print(std::ostream &stream) const {
    stream << *dereference();
}


bool ProcedureNodeReference::isResolved() const {
    return (procedure_ != nullptr);
}

void ProcedureNodeReference::resolve(ProcedureNode *procedure) {
    procedure_ = procedure;
}

ProcedureNode * ProcedureNodeReference::dereference() const {
    return procedure_;
}

void ProcedureNodeReference::addParameter(std::unique_ptr<ExpressionNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

void ProcedureNodeReference::setParameter(size_t num, std::unique_ptr<ExpressionNode> parameter) {
    parameters_[num] = std::move(parameter);
}

ExpressionNode * ProcedureNodeReference::getParameter(size_t num) const {
    return parameters_.at(num).get();
}

size_t ProcedureNodeReference::getParameterCount() const {
    return parameters_.size();
}


bool FunctionCallNode::isConstant() const {
    return false;
}

TypeNode * FunctionCallNode::getType() const {
    return this->dereference()->getReturnType();
}

void FunctionCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void FunctionCallNode::print(std::ostream &stream) const {
    stream << this->dereference()->getName() << "()";
}


std::string ProcedureCallNode::getName() const {
    return name_;
}

void ProcedureCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureCallNode::print(std::ostream &stream) const {
    stream << this->dereference()->getName() << "()";
}