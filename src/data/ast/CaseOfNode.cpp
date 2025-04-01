//
// Created by Michael Grossniklaus on 1/11/25.
//

#include "CaseOfNode.h"
#include "NodeVisitor.h"

#include <iostream>
#include <string>

using std::ostream;
using std::string;

TypeNode *CaseLabelNode::getType() const {
    return labels_.empty() ? nullptr : labels_[0]->getType();
}

ExpressionNode *CaseLabelNode::getValue(size_t num) const {
    return labels_.at(num).get();
}

size_t CaseLabelNode::getValueCount() const {
    return labels_.size();
}

const set<int64_t> &CaseLabelNode::getValues() const {
    return values_;
}

void CaseLabelNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void CaseLabelNode::print(std::ostream &stream) const {
    string sep;
    for (auto& label : labels_) {
        stream << sep << *label;
        sep = ", ";
    }
}


CaseLabelNode *CaseNode::getLabel() const {
    return label_.get();
}

StatementSequenceNode* CaseNode::getStatements() const {
    return statements_.get();
}

void CaseNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void CaseNode::print(ostream &stream) const {
    stream << *label_ << " : " << *statements_;
}


ExpressionNode* CaseOfNode::getExpression() const {
    return expression_.get();
}

CaseNode* CaseOfNode::getCase(const size_t num) const {
    return cases_.at(num).get();
}

size_t CaseOfNode::getCaseCount() const {
    return cases_.size();
}

size_t CaseOfNode::getLabelCount() const {
    if (labels_ == 0) {
        for (auto &c : cases_) {
            size_t values = c->getLabel()->getValues().size();
            labels_ += values > 0 ? values : 1;
        }
    }
    return labels_;
}

StatementSequenceNode* CaseOfNode::getElseStatements() const {
    return elseStatements_.get();
}

bool CaseOfNode::hasElse() const {
    return elseStatements_->getStatementCount() > 0;
}

bool CaseOfNode::hasExit() {
    bool res = false;
    for (size_t i = 0; i < cases_.size(); ++i) {
        res = res || cases_[i]->getStatements()->hasExit();
    }
    res = res || elseStatements_->hasExit();
    return res;
}

bool CaseOfNode::isReturn() {
    bool res = true;
    for (size_t i = 0; i < cases_.size(); ++i) {
        res = res && cases_[i]->getStatements()->isReturn();
    }
    if (elseStatements_->getStatementCount() > 0) {
        res = res && elseStatements_->isReturn();
    } else {
        // A case statement without an else can always fail to return
        return false;
    }
    return res;
}

bool CaseOfNode::isTerminator() {
    if (isReturn()) {
        return true;
    }
    bool res = true;
    for (size_t i = 0; i < cases_.size(); ++i) {
        res = res && cases_[i]->getStatements()->hasTerminator();
    }
    if (elseStatements_->getStatementCount() > 0) {
        res = res && elseStatements_->hasTerminator();
    } else {
        // A case statement without an else can always fail to return
        return false;
    }
    return res;
}

void CaseOfNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void CaseOfNode::print(ostream &stream) const {
    stream << "CASE " << expression_ << " OF ";
    string sep;
    for (auto& c : cases_) {
        stream << sep << *c;
        sep = " | ";
    }
    if (hasElse()) {
        stream << " ELSE " << *elseStatements_;
    }
    stream << " END;";
}