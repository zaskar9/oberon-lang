/*
 * Implementation of the AST record type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */
#include "RecordTypeNode.h"

RecordTypeNode::RecordTypeNode(const FilePos pos) : TypeNode(NodeType::record_type, pos, 0), fields_() {
}

RecordTypeNode::~RecordTypeNode() = default;

void RecordTypeNode::addField(std::unique_ptr<const NamedValueNode> field) {
    fields_.push_back(std::move(field));
}

const int RecordTypeNode::getSize() const {
    int size = 0;
    for (auto&& itr : fields_) {
        size += itr->getType()->getSize();
    }
    return size;
}

void RecordTypeNode::print(std::ostream &out) const {
    out << "RECORD";
}