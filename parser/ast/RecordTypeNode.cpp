/*
 * Implementation of the AST record type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "RecordTypeNode.h"
#include "NodeVisitor.h"

RecordTypeNode::RecordTypeNode(const FilePos pos) : TypeNode(NodeType::record_type, pos, 0), offset_(0), fields_() {
}

RecordTypeNode::~RecordTypeNode() = default;

void RecordTypeNode::addField(std::unique_ptr<FieldNode> field) {
    fields_.push_back(std::move(field));
}

int RecordTypeNode::getSize() const {
    int size = 0;
    for (auto&& itr : fields_) {
        size += itr->getType()->getSize();
    }
    return size;
}

int RecordTypeNode::getOffset() const {
    return offset_;
}

void RecordTypeNode::incOffset(int offset) {
    offset_ += offset;
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