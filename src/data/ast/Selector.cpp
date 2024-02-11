//
// Created by Michael Grossniklaus on 9/29/22.
//

#include "Selector.h"
#include "NodeReference.h"
#include "ExpressionNode.h"

Selector::~Selector() = default;

FilePos Selector::pos() const {
    return pos_;
}

NodeType Selector::getType() const {
    return type_;
}


ArrayIndex::ArrayIndex(const FilePos &pos, std::vector<std::unique_ptr<ExpressionNode>> indices) :
        Selector(NodeType::array_type, pos), indices_(std::move(indices)) { }

ArrayIndex::~ArrayIndex() = default;

const vector<unique_ptr<ExpressionNode>> &ArrayIndex::indices() const {
    return indices_;
}


RecordField::RecordField(const FilePos &pos, std::unique_ptr<Ident> ident) :
        Selector(NodeType::record_type, pos), ident_(std::move(ident)), field_() { }

RecordField::RecordField(const FilePos &pos, FieldNode *field) :
        Selector(NodeType::record_type, pos), ident_(), field_(field) { }

RecordField::~RecordField() = default;

Ident *RecordField::ident() const {
    return ident_ == nullptr ? field_->getIdentifier() : ident_.get();
}

void RecordField::setField(FieldNode *field) {
    field_ = field;
}

FieldNode *RecordField::getField() const {
    return field_;
}


Dereference::Dereference(const FilePos &pos) :
        Selector(NodeType::pointer_type, pos) { }

Dereference::~Dereference() = default;


Typeguard::Typeguard(const FilePos &pos, std::unique_ptr<QualIdent> ident)  :
        Selector(NodeType::type, pos), ident_(std::move(ident)) { }

Typeguard::~Typeguard() = default;

QualIdent *Typeguard::ident() const {
    return ident_.get();
}


ActualParameters::ActualParameters(const FilePos &pos, std::vector<std::unique_ptr<ExpressionNode>> parameters) :
        Selector(NodeType::parameter, pos), parameters_(std::move(parameters)) { }

ActualParameters::~ActualParameters() = default;

vector<unique_ptr<ExpressionNode>> &ActualParameters::parameters() {
    return parameters_;
}