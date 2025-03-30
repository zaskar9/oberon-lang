//
// Created by Michael Grossniklaus on 10/1/22.
//

#include "PointerTypeNode.h"
#include "NodeVisitor.h"

void PointerTypeNode::setBase(TypeNode *base) {
    base_ = base;
}

TypeNode *PointerTypeNode::getBase() const {
    return base_;
}

bool PointerTypeNode::extends(TypeNode *base) const {
    if (auto ptr = dynamic_cast<PointerTypeNode *>(base)) {
        return base_ ? base_->extends(ptr->getBase()) : false;
    }
    return false;
}

void PointerTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void PointerTypeNode::print(std::ostream &out) const {
    if (this->isAnonymous()) {
        out << "pointer type";
    } else {
        out << this->getIdentifier()->name();
    }
}