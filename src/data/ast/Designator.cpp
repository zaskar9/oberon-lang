//
// Created by Michael Grossniklaus on 9/29/22.
//

#include "Designator.h"
#include "NodeReference.h"
#include "ExpressionNode.h"

Selector::~Selector() = default;

FilePos Selector::pos() const {
    return pos_;
}

NodeType Selector::getNodeType() const {
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

void Typeguard::setType(TypeNode *type) {
    type_ = type;
}

TypeNode *Typeguard::getType() const {
    return type_;
}


ActualParameters::ActualParameters(const FilePos &pos, std::vector<std::unique_ptr<ExpressionNode>> parameters) :
        Selector(NodeType::parameter, pos), proc_(), parameters_(std::move(parameters)) { }

ActualParameters::ActualParameters() :
        Selector(NodeType::parameter, EMPTY_POS), proc_(), parameters_() {}

ActualParameters::~ActualParameters() = default;

void ActualParameters::setProcedure(ProcedureNode *proc) {
    proc_ = proc;
}

ProcedureNode *ActualParameters::getProcedure() const {
    return proc_;
}

vector<unique_ptr<ExpressionNode>> &ActualParameters::parameters() {
    return parameters_;
}


Designator::~Designator() = default;

QualIdent *Designator::ident() const {
    return ident_.get();
}

vector<unique_ptr<Selector>> &Designator::selectors() {
    return selectors_;
}

void Designator::addSelector(std::unique_ptr<Selector> selector) {
    selectors_.push_back(std::move(selector));
}

void Designator::insertSelector(size_t num, std::unique_ptr<Selector> selector) {
    selectors_.insert(selectors_.begin() + (long) num, std::move(selector));
}

void Designator::setSelector(size_t num, std::unique_ptr<Selector> selector) {
    selectors_[num] = std::move(selector);
}

Selector *Designator::getSelector(size_t num) const {
    return selectors_[num].get();
}

void Designator::removeSelector(size_t num) {
    selectors_.erase(selectors_.begin() + (long) num);
}

size_t Designator::getSelectorCount() const {
    return selectors_.size();
}
