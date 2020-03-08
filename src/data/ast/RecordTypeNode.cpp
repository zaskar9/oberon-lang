/*
 * AST node representing a record type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "RecordTypeNode.h"
#include "NodeVisitor.h"

void RecordTypeNode::addField(std::unique_ptr<FieldNode> field) {
    fields_.push_back(std::move(field));
}

unsigned int RecordTypeNode::getSize() const {
    unsigned int size = 0;
    for (auto&& itr : fields_) {
        size += itr->getType()->getSize();
    }
    return size;
}

FieldNode* RecordTypeNode::getField(const std::string& name) const {
    for (auto&& itr : fields_) {
        if (itr->getName() == name) {
            return itr.get();
        }
    }
    return nullptr;
}

FieldNode* RecordTypeNode::getField(size_t num) const {
    return fields_.at(num).get();
}

size_t RecordTypeNode::getFieldCount() {
    return fields_.size();
}

void RecordTypeNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void RecordTypeNode::print(std::ostream& stream) const {
    stream << "RECORD ";
}