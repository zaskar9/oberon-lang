//
// Created by Michael Grossniklaus on 10/1/22.
//

#include "PointerTypeNode.h"
#include "NodeVisitor.h"


PointerTypeNode::PointerTypeNode(const FilePos &pos, TypeNode *base) :
        TypeNode(NodeType::pointer_type, pos, TypeKind::POINTER, 8) {
    setBase(base);
}

void PointerTypeNode::setBase(TypeNode *base) {
    base_ = base;
    if (const auto record = dynamic_cast<RecordTypeNode*>(base)) {
        record->setParent(this);
    }
}

TypeNode *PointerTypeNode::getBase() const {
    return base_;
}

bool PointerTypeNode::extends(TypeNode *base) const {
    if (const auto ptr = dynamic_cast<PointerTypeNode *>(base)) {
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