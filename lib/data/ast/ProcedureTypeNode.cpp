//
// Created by Michael Grossniklaus on 3/17/22.
//

#include "ProcedureTypeNode.h"
#include "NodeVisitor.h"
#include <algorithm>

vector<unique_ptr<ParameterNode>> &ProcedureTypeNode::parameters() {
    return parameters_;
}

bool ProcedureTypeNode::hasVarArgs() const {
    return varargs_;
}

void ProcedureTypeNode::setReturnType(TypeNode *type) {
    type_ = type;
}

TypeNode *ProcedureTypeNode::getReturnType() const {
    return type_;
}

bool ProcedureTypeNode::isProper() const {
    return !isFunction();
}

bool ProcedureTypeNode::isFunction() const {
    return type_ && type_->kind() != TypeKind::NOTYPE;
}


void ProcedureTypeNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void ProcedureTypeNode::print(std::ostream &out) const {
    if (this->isAnonymous()) {
        out << "procedure type";
    } else {
        out << *this->getIdentifier();
    }
}

