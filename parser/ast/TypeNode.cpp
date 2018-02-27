/*
 * Implementation of the AST type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "TypeNode.h"

TypeNode::TypeNode(const NodeType type, const FilePos pos, const int size) :
        Node(type, pos), size_(size) {
}

TypeNode::~TypeNode() = default;

const int TypeNode::getSize() const {
    return size_;
}
