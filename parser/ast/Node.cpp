/*
 * Implementation of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */


#include "Node.h"

Node::Node(const NodeType nodeType, const FilePos pos) : nodeType_(nodeType), pos_(pos) {
}

Node::~Node() = default;

const NodeType Node::getNodeType() const {
    return type_;
}

const FilePos Node::getFilePos() const {
    return pos_;
}

std::ostream& operator<<(std::ostream &stream, const Node &symbol) {
    symbol.print(stream);
    return stream;
}