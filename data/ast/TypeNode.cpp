/*
 * AST node representing a type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "TypeNode.h"

std::string TypeNode::getName() const {
    return name_;
}

unsigned int TypeNode::getSize() const {
    return size_;
}

bool TypeNode::isAnonymous() {
    return anon_;
}
