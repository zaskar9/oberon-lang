/*
 * AST node representing a reference to a declaration in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "NodeReference.h"
#include "NodeVisitor.h"

NodeReference::~NodeReference() = default;

Identifier *ValueReferenceNode::getIdentifier() const {
    if (isResolved()) {
        return dereference()->getIdentifier();
    }
    return ident_.get();
}

bool ValueReferenceNode::isResolved() const {
    return (node_ != nullptr);
}

void ValueReferenceNode::resolve(DeclarationNode *node) {
    node_ = node;
    this->setType(node->getType());
}

DeclarationNode *ValueReferenceNode::dereference() const {
    return node_;
}

void ValueReferenceNode::addSelector(NodeType nodeType, std::unique_ptr<ExpressionNode> selector) {
    selectors_.push_back(std::move(selector));
    types_.push_back(nodeType);
}

void ValueReferenceNode::insertSelector(size_t num, NodeType nodeType, std::unique_ptr<ExpressionNode> selector) {
    selectors_.insert(selectors_.begin() + (long) num, std::move(selector));
    types_.insert(types_.begin() + (long) num, nodeType);
}

void ValueReferenceNode::setSelector(size_t num, std::unique_ptr<ExpressionNode> selector) {
    selectors_[num] = std::move(selector);
}

ExpressionNode *ValueReferenceNode::getSelector(size_t num) const {
    return selectors_[num].get();
}

NodeType ValueReferenceNode::getSelectorType(size_t num) const {
    return types_[num];
}

size_t ValueReferenceNode::getSelectorCount() const {
    return selectors_.size();
}

bool ValueReferenceNode::isConstant() const {
    if (isResolved()) {
        return dereference()->getNodeType() == NodeType::constant;
    } else {
        return false;
    }
}

int ValueReferenceNode::getPrecedence() const {
    return 4;
}


void ValueReferenceNode::accept(NodeVisitor &visitor) {
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

TypeNode *TypeReferenceNode::dereference() const {
    return node_;
}

TypeKind TypeReferenceNode::kind() const {
    if (node_) {
        return node_->kind();
    }
    return TypeNode::kind();
}

unsigned int TypeReferenceNode::getSize() const {
    if (node_) {
        return node_->getSize();
    }
    return TypeNode::getSize();
}

void TypeReferenceNode::accept(NodeVisitor &visitor) {
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

ProcedureNode *ProcedureNodeReference::dereference() const {
    return procedure_;
}

void ProcedureNodeReference::addActualParameter(std::unique_ptr<ExpressionNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

void ProcedureNodeReference::setActualParameter(size_t num, std::unique_ptr<ExpressionNode> parameter) {
    parameters_[num] = std::move(parameter);
}

ExpressionNode *ProcedureNodeReference::getActualParameter(size_t num) const {
    return parameters_.at(num).get();
}

size_t ProcedureNodeReference::getActualParameterCount() const {
    return parameters_.size();
}

bool FunctionCallNode::isConstant() const {
    return false;
}

TypeNode *FunctionCallNode::getType() const {
    if (ProcedureNodeReference::isResolved()) {
        return this->dereference()->getReturnType();
    }
    return nullptr;
}

void FunctionCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void FunctionCallNode::print(std::ostream &stream) const {
    stream << this->dereference()->getIdentifier() << "()";
}


Identifier *ProcedureCallNode::getIdentifier() const {
    if (isResolved()) {
        return dereference()->getIdentifier();
    }
    return ident_.get();
}

void ProcedureCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureCallNode::print(std::ostream &stream) const {
    stream << this->dereference()->getIdentifier() << "()";
}