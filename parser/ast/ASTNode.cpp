/*
 * Implementation of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include "ASTNode.h"

ASTNode::ASTNode(const NodeType type, const FilePos pos) : type_(type), pos_(pos) {
}

ASTNode::~ASTNode() = default;

const NodeType ASTNode::getNodeType() const {
    return type_;
}

const FilePos ASTNode::getFilePos() const {
    return pos_;
}

std::ostream& operator<<(std::ostream &stream, const ASTNode &symbol) {
    symbol.print(stream);
    return stream;
}