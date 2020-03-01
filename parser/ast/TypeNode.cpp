/*
 * Implementation of the AST type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "TypeNode.h"

TypeNode::~TypeNode() = default;

void TypeNode::setName(std::string name) {
    name_ = std::move(name);
}

std::string TypeNode::getName() const {
    return name_;
}

unsigned int TypeNode::getSize() const {
    return size_;
}
