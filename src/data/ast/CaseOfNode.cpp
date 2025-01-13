//
// Created by Michael Grossniklaus on 1/11/25.
//

#include "CaseOfNode.h"
#include "NodeVisitor.h"

#include <iostream>
#include <string>

using std::ostream;
using std::string;

ExpressionNode* CaseNode::getLabel(const size_t num) const {
    return labels_.at(num).get();
}

size_t CaseNode::getLabelCount() const {
    return labels_.size();
}

StatementSequenceNode* CaseNode::getStatements() const {
    return statements_.get();
}

void CaseNode::accept(NodeVisitor &visitor) {
    visitor.visit(*this);
}

void CaseNode::print(ostream &stream) const {
    string sep;
    for (auto& label : labels_) {
        stream << sep << *label;
        sep = ", ";
    }
    stream << " : " << *statements_;
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

StatementSequenceNode* CaseOfNode::getElseStatements() const {
    return elseStatements_.get();
}

bool CaseOfNode::hasElse() const {
    return elseStatements_->getStatementCount() > 0;
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