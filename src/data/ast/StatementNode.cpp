/*
 * AST node representing a statement in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "StatementNode.h"
#include "NodeVisitor.h"

StatementNode::~StatementNode() = default;

bool StatementNode::hasExit() {
    return false;
}

bool StatementNode::isReturn() {
    return false;
}

bool StatementNode::isTerminator() {
    return isReturn();
}

ExpressionNode *ReturnNode::getValue() const {
    return value_.get();
}

bool ReturnNode::isReturn() {
    return true;
}

void ReturnNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ReturnNode::print(std::ostream &stream) const {
    stream << "RETURN " << *value_;
}

bool ExitNode::hasExit() {
    return true;
}

bool ExitNode::isTerminator() {
    return true;
}

void ExitNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ExitNode::print(std::ostream &stream) const {
    stream << "EXIT";
}
