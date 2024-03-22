/*
 * AST node representing a type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "TypeNode.h"
#include "DeclarationNode.h"

std::ostream &operator<<(std::ostream &stream, const TypeKind &kind) {
    std::string result;
    switch (kind) {
        case TypeKind::ANYTYPE: result = "ANYTYPE"; break;
        case TypeKind::NOTYPE: result = "NOTYPE"; break;
        case TypeKind::NILTYPE: result = "NILTYPE"; break;
        case TypeKind::ENTIRE: result = "integer type"; break;
        case TypeKind::FLOATING: result = "floating-point type"; break;
        case TypeKind::NUMERIC: result = "numeric type"; break;
        case TypeKind::ARRAY: result = "ARRAY"; break;
        case TypeKind::POINTER: result = "POINTER"; break;
        case TypeKind::PROCEDURE: result = "PROCEDURE"; break;
        case TypeKind::RECORD: result = "RECORD"; break;
        case TypeKind::SET: result = "SET"; break;
        case TypeKind::BOOLEAN: result = "BOOLEAN"; break;
        case TypeKind::BYTE: result = "BYTE"; break;
        case TypeKind::CHAR: result = "CHAR"; break;
        case TypeKind::SHORTINT: result = "SHORTINT"; break;
        case TypeKind::INTEGER: result = "INTEGER"; break;
        case TypeKind::LONGINT: result = "LONGINT"; break;
        case TypeKind::REAL: result = "REAL"; break;
        case TypeKind::LONGREAL: result = "LONGREAL"; break;
        case TypeKind::STRING: result = "STRING"; break;
        default: result = "UNKNOWN"; break;
    }
    stream << result;
    return stream;
}

void TypeNode::setDeclaration(TypeDeclarationNode *decl) {
    decl_ = decl;
}

TypeDeclarationNode *TypeNode::getDeclaration() const {
    return decl_ ;
}

Ident *TypeNode::getIdentifier() const {
    return decl_ ? decl_->getIdentifier() : nullptr;
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
    return decl_ == nullptr && !isBasic();
}

bool TypeNode::isArray() const {
    return kind_ == TypeKind::ARRAY;
}

bool TypeNode::isRecord() const {
    return kind_ == TypeKind::RECORD;
}

bool TypeNode::isStructured() const {
    return isArray() || isRecord();
}

bool TypeNode::isPointer() const {
    return kind_ == TypeKind::POINTER;
}

bool TypeNode::isProcedure() const {
    return kind_ == TypeKind::PROCEDURE;
}

bool TypeNode::isBoolean() const {
    return kind_ == TypeKind::BOOLEAN;
}

bool TypeNode::isInteger() const {
    return kind_ == TypeKind::SHORTINT ||
           kind_ == TypeKind::INTEGER ||
           kind_ == TypeKind::LONGINT ||
           kind_ == TypeKind::ENTIRE;
}

bool TypeNode::isReal() const {
    return kind_ == TypeKind::REAL ||
           kind_ == TypeKind::LONGREAL ||
           kind_ == TypeKind::FLOATING;
}

bool TypeNode::isNumeric() const {
    return isInteger() ||
           isReal() ||
           kind_ == TypeKind::NUMERIC;
}

bool TypeNode::isString() const {
    return kind_ == TypeKind::STRING;
}

bool TypeNode::isSet() const {
    return kind_ == TypeKind::SET;
}

bool TypeNode::isChar() const {
    return kind_ == TypeKind::CHAR;
}

bool TypeNode::isVirtual() const {
    return kind_ == TypeKind::ANYTYPE ||
           kind_ == TypeKind::NOTYPE ||
           kind_ == TypeKind::TYPE ||
           kind_ == TypeKind::NUMERIC ||
           kind_ == TypeKind::ENTIRE ||
           kind_ == TypeKind::FLOATING;
}

bool TypeNode::isBasic() const {
    return isBoolean() || isNumeric() || isSet();
}

void TypeNode::setRef(int ref) {
    ref_ = ref;
}

int TypeNode::getRef() const {
    return ref_;
}