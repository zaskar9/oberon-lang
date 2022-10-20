/*
 * AST node representing a type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "TypeNode.h"

Ident *TypeNode::getIdentifier() const {
    return ident_;
}

TypeKind TypeNode::kind() const {
    return kind_;
}

void TypeNode::setSize(unsigned int size) {
    size_ = size;
}

unsigned int TypeNode::getSize() const {
    return size_;
}

bool TypeNode::isAnonymous() const {
    return anon_;
}

bool TypeNode::isArray() const {
    return kind_ == TypeKind::ARRAY;
}

bool TypeNode::isRecord() const {
    return kind_ == TypeKind::RECORD;
}

bool TypeNode::isPointer() const {
    return kind_ == TypeKind::POINTER;
}

bool TypeNode::isBoolean() const {
    return kind_ == TypeKind::BOOLEAN;
}

bool TypeNode::isNumeric() const {
    return this->isInteger() || this->isReal();
}

bool TypeNode::isInteger() const {
    return kind_ == TypeKind::BYTE || kind_ == TypeKind::CHAR ||
           kind_ == TypeKind::INTEGER || kind_ == TypeKind::LONGINT;
}

bool TypeNode::isReal() const {
    return kind_ == TypeKind::REAL || kind_ == TypeKind::LONGREAL;
}

bool TypeNode::isString() const {
    return kind_ == TypeKind::STRING;
}

void TypeNode::setRef(int ref) {
    ref_ = ref;
}

int TypeNode::getRef() const {
    return ref_;
}