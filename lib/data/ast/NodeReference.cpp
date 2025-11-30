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

bool QualifiedExpression::isVarParameter() {
    if (selectors().empty()) {
        if (const auto decl = dereference()) {
            if (decl->getNodeType() == NodeType::parameter) {
                const auto param = dynamic_cast<const ParameterNode *>(decl);
                return param->isVar();
            }
        }
    }
    return false;
}


FilePos QualifiedExpression::pos() const {
    return ExpressionNode::pos();
}

bool QualifiedExpression::isConstant() const {
    const auto node = dereference();
    return node && node->getNodeType() == NodeType::constant;
}

int QualifiedExpression::getPrecedence() const {
    // TODO magic number!
    return 4;
}

TypeNode *QualifiedExpression::getType() const {
    return ExpressionNode::getType();;
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

FilePos QualifiedStatement::pos() const {
    return StatementNode::pos();
}

void QualifiedStatement::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void QualifiedStatement::print(std::ostream &stream) const {
    stream << *dereference();
}