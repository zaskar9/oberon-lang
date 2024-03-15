/*
 * AST node representing a record type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "RecordTypeNode.h"
#include "NodeVisitor.h"

unsigned int RecordTypeNode::getSize() const {
    unsigned int size = 0;
    for (auto &&itr : fields_) {
        size += itr->getType()->getSize();
    }
    return size;
}

FieldNode *RecordTypeNode::getField(const std::string &name) const {
    for (auto &&itr : fields_) {
        if (itr->getIdentifier()->name() == name) {
            return itr.get();
        }
    }
    return nullptr;
}

FieldNode *RecordTypeNode::getField(size_t num) const {
    return fields_.at(num).get();
}

size_t RecordTypeNode::getFieldCount() {
    return fields_.size();
}

RecordTypeNode *RecordTypeNode::getBaseType() const {
    return base_;
}

bool RecordTypeNode::isExtened() const {
    return base_ != nullptr;
}

bool RecordTypeNode::extends(TypeNode *base) const {
    if (this != base && base_) {
        return base_->extends(base);
    }
    return this == base;
}

unsigned short RecordTypeNode::level() const {
    return level_;
}

void RecordTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void RecordTypeNode::print(std::ostream &out) const {
    if (this->isAnonymous()) {
        out << "record type";
    } else {
        out << *this->getIdentifier();
    }
}