/*
 * AST node representing a record type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "RecordTypeNode.h"
#include "NodeVisitor.h"

RecordTypeNode::RecordTypeNode(const FilePos &pos, RecordTypeNode *base, vector<unique_ptr<FieldNode>> fields) :
        TypeNode(NodeType::record_type, pos, TypeKind::RECORD, 0),
        fields_(std::move(fields)), base_(base), level_(base ? base->getLevel() + 1 : 0) {
    unsigned index = 0;
    for (auto& field : fields_) {
        field->setRecordType(this);
        field->setIndex(index++);
    }
}

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
    if (base_) {
        return base_->getField(name);
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

bool RecordTypeNode::isExtended() const {
    return base_ != nullptr;
}

bool RecordTypeNode::extends(TypeNode *base) const {
    if (this != base && base_) {
        return base_->extends(base);
    }
    return this == base;
}

unsigned short RecordTypeNode::getLevel() const {
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