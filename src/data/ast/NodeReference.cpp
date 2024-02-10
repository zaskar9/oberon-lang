/*
 * AST node representing a reference to a declaration in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "NodeReference.h"
#include "NodeVisitor.h"


NodeReference::~NodeReference() = default;


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


Designator::~Designator() = default;

QualIdent *Designator::ident() const {
    return ident_.get();
}

void Designator::addSelector(std::unique_ptr<Selector> selector) {
    selectors_.push_back(std::move(selector));
}

void Designator::insertSelector(size_t num, std::unique_ptr<Selector> selector) {
    selectors_.insert(selectors_.begin() + (long) num, std::move(selector));
}

void Designator::setSelector(size_t num, std::unique_ptr<Selector> selector) {
    selectors_[num] = std::move(selector);
}

Selector *Designator::getSelector(size_t num) const {
    return selectors_[num].get();
}

void Designator::removeSelector(size_t num) {
    selectors_.erase(selectors_.begin() + (long) num);
}

size_t Designator::getSelectorCount() const {
    return selectors_.size();
}

void Designator::disqualify() {
    if (ident_->isQualified()) {
        auto qual = dynamic_cast<QualIdent *>(ident_.get());
        auto pos = ident_->start();
        pos.charNo += ((int) qual->qualifier().size()) + 1;
        auto field = std::make_unique<QualIdent>(pos, EMPTY_POS, qual->name());
        // TODO no AST node creation outside sema
        this->insertSelector(0, std::make_unique<RecordField>(field->start(), std::move(field)));
        ident_ = std::make_unique<QualIdent>(qual->qualifier());
    }
}


ProcedureNodeReference::~ProcedureNodeReference() = default;

void ProcedureNodeReference::initActualParameters() {
    parameters_.clear();
    if (this->getSelectorCount() > 0) {
        auto selector = this->getSelector(0);
        if (selector->getType() == NodeType::parameter) {
            auto parameters = dynamic_cast<ActualParameters *>(selector);
            parameters->moveActualParameters(parameters_);
            this->removeSelector(0);
        }
    }
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


ValueReferenceNode::ValueReferenceNode(const FilePos &pos, DeclarationNode *node)  :
        ExpressionNode(NodeType::value_reference, pos, nullptr),
        ProcedureNodeReference(std::make_unique<Designator>(std::make_unique<QualIdent>(node->getIdentifier()))),
        node_() {
    this->resolve(node);
}

void ValueReferenceNode::resolve(DeclarationNode *node) {
    node_ = node;
    auto type = node->getType();
    if (type && (type->getNodeType() == NodeType::procedure || type->getNodeType() == NodeType::procedure_type)) {
        this->initActualParameters();
        this->setNodeType(NodeType::procedure_call);
    }
    this->setType(type);
}

bool ValueReferenceNode::isResolved() const {
    return (node_ != nullptr);
}

DeclarationNode *ValueReferenceNode::dereference() const {
    return node_;
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

TypeNode *ValueReferenceNode::getType() const {
    if (this->isResolved()) {
        if (this->getNodeType() == NodeType::procedure_call) {
            auto proc = dynamic_cast<ProcedureNode *>(this->dereference());
            return proc->getReturnType();
        }
        return ExpressionNode::getType();
    }
    return nullptr;
}

void ValueReferenceNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ValueReferenceNode::print(std::ostream &stream) const {
    stream << *dereference();
}


void ProcedureCallNode::resolve(DeclarationNode *node) {
    node_ = dynamic_cast<ProcedureNode *>(node);
    this->initActualParameters();
}

bool ProcedureCallNode::isResolved() const {
    return (node_ != nullptr);
}

ProcedureNode *ProcedureCallNode::dereference() const {
    return node_;
}

void ProcedureCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureCallNode::print(std::ostream &stream) const {
    stream << this->dereference()->getIdentifier() << "()";
}