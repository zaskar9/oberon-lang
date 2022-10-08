//
// Created by Michael Grossniklaus on 9/29/22.
//

#include "Selector.h"
#include "NodeReference.h"
#include "ExpressionNode.h"

Selector::~Selector() = default;

NodeType Selector::getType() const {
    return type_;
}

ArraySelector::ArraySelector(std::unique_ptr<ExpressionNode> expression) :
        Selector(NodeType::array_type), expression_(std::move(expression)) { }

ArraySelector::~ArraySelector() = default;

FilePos ArraySelector::pos() const {
    return expression_->pos();
}

ExpressionNode *ArraySelector::getExpression() const {
    return expression_.get();
}

RecordSelector::RecordSelector(std::unique_ptr<ValueReferenceNode> field) :
        Selector(NodeType::record_type), ident_(std::move(field)) { }

RecordSelector::~RecordSelector() = default;

FilePos RecordSelector::pos() const {
    return ident_->pos();
}

ValueReferenceNode *RecordSelector::getField() const {
    return ident_.get();
}

CaretSelector::~CaretSelector() = default;

FilePos CaretSelector::pos() const {
    return pos_;
}
