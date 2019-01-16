/*
 * Implementation of the AST literal nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */


#include "LiteralNode.h"
#include "NodeVisitor.h"

bool LiteralNode::isConstant() const {
    return true;
}

BasicTypeNode* LiteralNode::getType() const {
    return type_;
}


bool BooleanLiteralNode::getValue() const {
    return value_;
}

void BooleanLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void BooleanLiteralNode::print(std::ostream &stream) const {
    stream << (value_ ? "TRUE" : "FALSE");
}


int IntegerLiteralNode::getValue() const {
    return value_;
}

void IntegerLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void IntegerLiteralNode::print(std::ostream &stream) const {
    stream << value_;
}


const std::string StringLiteralNode::getValue() const {
    return value_;
}

void StringLiteralNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void StringLiteralNode::print(std::ostream &stream) const {
    stream << value_;
}

