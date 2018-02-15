/*
 * Implementation of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include "ASTNode.h"

ASTNode::ASTNode(const NodeType type) : type_(type), next_(nullptr) {
}

ASTNode::~ASTNode() = default;

const NodeType ASTNode::getNodeType() const {
    return type_;
}

void ASTNode::setNext(ASTNode *next) {
    next_ = next;
}

const ASTNode* ASTNode::getNext() const {
    return next_;
}

std::ostream& operator<<(std::ostream &stream, const ASTNode &symbol) {
    symbol.print(stream);
    return stream;
}