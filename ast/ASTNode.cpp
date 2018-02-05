/*
 * Implementation of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include "ASTNode.h"

ASTNode::ASTNode(const NodeType type) : next_(NULL), firstChild_(NULL), lastChild_(NULL) {
    type_ = type;
}

ASTNode::~ASTNode() {
    if (firstChild_ != NULL) {
        const ASTNode *next = firstChild_;
        while (next != NULL) {
            const ASTNode *previous = next;
            next = previous->getNext();
            delete previous;
        }
    }
}

const NodeType ASTNode::getNodeType() const {
    return type_;
}

void ASTNode::setNext(ASTNode *next) {
    next_ = next;
}

const ASTNode* ASTNode::getNext() const {
    return next_;
}

const ASTNode* ASTNode::getFirstChild() const {
    return firstChild_;
}

void ASTNode::addChild(ASTNode *child) {
    if (firstChild_ == NULL) {
        firstChild_ = child;
        lastChild_ = child;
    } else {
        lastChild_->setNext(child);
        lastChild_ = child;
    }
}