/*
 * AST node representing a reference to a declaration in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "NodeReference.h"
#include "NodeVisitor.h"


NodeReference::~NodeReference() = default;

void NodeReference::resolve(DeclarationNode *node) {
    node_ = node;
}

bool NodeReference::isResolved() const {
    return node_ != nullptr;
}

DeclarationNode *NodeReference::dereference() const {
    return node_;
}


ProcedureNodeReference::~ProcedureNodeReference() = default;

void ProcedureNodeReference::initActualParameters() {
    if (this->getSelectorCount() > 0) {
        auto selector = this->getSelector(0);
        if (selector->getType() == NodeType::parameter) {
            auto parameters = dynamic_cast<ActualParameters *>(selector);
            parameters_ = std::move(parameters->parameters());
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


ValueReferenceNode::ValueReferenceNode(const FilePos &pos, DeclarationNode *node) :
        ExpressionNode(NodeType::value_reference, pos, nullptr),
        ProcedureNodeReference(std::make_unique<Designator>(std::make_unique<QualIdent>(node->getIdentifier()))) {
    this->resolve(node);
}

void ValueReferenceNode::resolve(DeclarationNode *node) {
    NodeReference::resolve(node);
    auto type = node->getType();
    if (type && (type->getNodeType() == NodeType::procedure || type->getNodeType() == NodeType::procedure_type)) {
        this->initActualParameters();
        this->setNodeType(NodeType::procedure_call);
    }
    this->setType(type);
    // TODO this method should also update the designator to match the new declaration
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
    NodeReference::resolve(node);
    this->initActualParameters();
}

void ProcedureCallNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureCallNode::print(std::ostream &stream) const {
    stream << this->dereference()->getIdentifier() << "()";
}


QualifiedExpression::~QualifiedExpression() = default;

bool QualifiedExpression::isConstant() const {
    return dereference()->getNodeType() == NodeType::constant;
}

int QualifiedExpression::getPrecedence() const {
    return 4;   // TODO magic number!
}

TypeNode *QualifiedExpression::getType() const {
    auto type = ExpressionNode::getType();
    if (type->kind() == TypeKind::PROCEDURE) {
        auto proc = dynamic_cast<ProcedureTypeNode *>(type);
        return proc->getReturnType();
    }
    return type;
}

void QualifiedExpression::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void QualifiedExpression::print(std::ostream &stream) const {
    stream << *dereference();
}


QualifiedStatement::~QualifiedStatement() = default;