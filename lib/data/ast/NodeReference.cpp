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

DeclarationNode *NodeReference::dereference() const {
    return node_;
}


QualifiedExpression::~QualifiedExpression() = default;

bool QualifiedExpression::isConstant() const {
    return dereference()->getNodeType() == NodeType::constant;
}

int QualifiedExpression::getPrecedence() const {
    // TODO magic number!
    return 4;
}

TypeNode *QualifiedExpression::getType() const {
    auto type = ExpressionNode::getType();
//    if (type && type->kind() == TypeKind::PROCEDURE) {
//        auto proc = dynamic_cast<ProcedureTypeNode *>(type);
//        return proc->getReturnType();
//    }
    return type;
}

void QualifiedExpression::resolve(DeclarationNode *node) {
    NodeReference::resolve(node);
    setIdent(node->getIdentifier());
    setType(node->getType());
}

void QualifiedExpression::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void QualifiedExpression::print(std::ostream &stream) const {
    stream << *dereference();
}


QualifiedStatement::~QualifiedStatement() = default;

void QualifiedStatement::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void QualifiedStatement::print(std::ostream &stream) const {
    stream << *dereference();
}