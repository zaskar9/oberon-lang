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


ArrayIndex::ArrayIndex(const FilePos &pos, std::unique_ptr<ExpressionNode> expression) :
        Selector(NodeType::array_type, pos), expression_(std::move(expression)) { }

ArrayIndex::~ArrayIndex() = default;

ExpressionNode *ArrayIndex::getExpression() const {
    return expression_.get();
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


ActualParameters::ActualParameters(const FilePos &pos) :
        Selector(NodeType::parameter, pos), parameters_() { }

ActualParameters::~ActualParameters() = default;

void ActualParameters::addActualParameter(std::unique_ptr<ExpressionNode> parameter) {
    parameters_.push_back(std::move(parameter));
}

void ActualParameters::moveActualParameters(std::vector<std::unique_ptr<ExpressionNode>> &target) {
    for (auto& parameter: parameters_) {
        target.push_back(std::move(parameter));
    }
}