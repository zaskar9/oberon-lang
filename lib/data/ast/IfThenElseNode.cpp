/*
 * AST node representing an if-then-elsif-else statement in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "IfThenElseNode.h"
#include "NodeVisitor.h"

ExpressionNode* ElseIfNode::getCondition() const {
    return condition_.get();
}

StatementSequenceNode* ElseIfNode::getStatements() const {
    return statements_.get();
}

void ElseIfNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ElseIfNode::print(std::ostream& stream) const {
    stream << "ELSIF " << *condition_ << " THEN " << *statements_;
}


ExpressionNode* IfThenElseNode::getCondition() const {
    return condition_.get();
}

StatementSequenceNode* IfThenElseNode::getThenStatements() const {
    return thenStatements_.get();
}

ElseIfNode* IfThenElseNode::getElseIf(const size_t num) const {
    return elseIfs_.at(num).get();
}

size_t IfThenElseNode::getElseIfCount() const {
    return elseIfs_.size();
}

bool IfThenElseNode::hasElseIf() const {
    return !elseIfs_.empty();
}

StatementSequenceNode* IfThenElseNode::getElseStatements() const {
    return elseStatements_.get();
}

bool IfThenElseNode::hasElse() const {
    return elseStatements_->getStatementCount() > 0;
}

bool IfThenElseNode::hasExit() {
    bool res = thenStatements_->hasExit();
    for (size_t i = 0; i < elseIfs_.size(); ++i) {
        res = res || elseIfs_.at(i)->getStatements()->hasExit();
    }
    res = res || elseStatements_->hasExit();
    return res;
}

bool IfThenElseNode::isReturn() {
    bool res = thenStatements_->isReturn();
    for (size_t i = 0; i < elseIfs_.size(); ++i) {
        res = res && elseIfs_.at(i)->getStatements()->isReturn();
    }
    res = res && elseStatements_->isReturn();
    return res;
}

bool IfThenElseNode::isTerminator() {
    if (isReturn()) {
        return true;
    }
    bool res = thenStatements_->hasTerminator();
    for (size_t i = 0; i < elseIfs_.size(); ++i) {
        res = res && elseIfs_.at(i)->getStatements()->hasTerminator();
    }
    res = res && elseStatements_->hasTerminator();
    return res;
}

void IfThenElseNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void IfThenElseNode::print(std::ostream& stream) const {
    stream << "IF " << *condition_ << " THEN " << *thenStatements_;
    for (auto const& elseIf : elseIfs_) {
        stream << *elseIf;
    }
    if (elseStatements_ != nullptr) {
        stream << " ELSE " << *elseStatements_;
    }
    stream << " END";
}
