/*
 * Implementation of the AST statement node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "StatementNode.h"
#include "NodeVisitor.h"

ExpressionNode * ReturnNode::getValue() const {
    return value_.get();
}

void ReturnNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ReturnNode::print(std::ostream &stream) const {
    stream << "RETURN " << *value_;
}

