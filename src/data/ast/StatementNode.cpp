/*
 * AST node representing a statement in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "StatementNode.h"
#include "NodeVisitor.h"

StatementNode::~StatementNode() = default;

ExpressionNode * ReturnNode::getValue() const {
    return value_.get();
}

void ReturnNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ReturnNode::print(std::ostream &stream) const {
    stream << "RETURN " << *value_;
}

