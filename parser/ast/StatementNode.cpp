/*
 * Implementation of the AST statement node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "StatementNode.h"

StatementNode::StatementNode(const NodeType type, const FilePos pos) : Node(type, pos) {
}

StatementNode::~StatementNode() = default;