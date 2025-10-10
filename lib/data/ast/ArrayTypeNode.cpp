/*
 * AST node representing an array type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#include "ArrayTypeNode.h"
#include "NodeVisitor.h"

unsigned int ArrayTypeNode::dimensions() const {
    return dimensions_;
}

const vector<unsigned int> &ArrayTypeNode::lengths() const {
    return lengths_;
}

const vector<TypeNode *> &ArrayTypeNode::types() const {
    return types_;
}

TypeNode *ArrayTypeNode::getElementType() const {
    if (!types_.empty()) {
        return types_[types_.size() - 1];
    }
    return nullptr;
}

void ArrayTypeNode::setBase(ArrayTypeNode *base) {
    base_ = base;
    for (auto& type : types_) {
        if (auto array_t = dynamic_cast<ArrayTypeNode *>(type)) {
            array_t->setBase(base);
        }
    }
}

ArrayTypeNode *ArrayTypeNode::getBase() const {
    return base_;
}

bool ArrayTypeNode::isOpen() const {
    return lengths_[0] == 0;
}

void ArrayTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ArrayTypeNode::print(std::ostream &out) const {
    if (this->isAnonymous()) {
        out << "array type";
    } else {
        out << this->getIdentifier()->name();
    }
}
