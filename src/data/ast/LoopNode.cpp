/*
 * AST nodes representing loop statements in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 01/14/19.
 */

#include "LoopNode.h"
#include "NodeVisitor.h"

StatementSequenceNode* LoopNode::getStatements() const {
    return statements_.get();
}

void LoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void LoopNode::print(std::ostream& stream) const {
    stream << "LOOP" << *statements_ << "END";
}

bool LoopNode::hasExit() {
    return statements_->hasExit();
}

bool LoopNode::isReturn() {
    return statements_->isReturn();
}


ConditionalLoopNode::~ConditionalLoopNode() = default;


ExpressionNode* ConditionalLoopNode::getCondition() const {
    return condition_.get();
}


ElseIfNode* WhileLoopNode::getElseIf(const size_t num) const {
    return elseIfs_.at(num).get();
}

size_t WhileLoopNode::getElseIfCount() const {
    return elseIfs_.size();
}

bool WhileLoopNode::hasElseIf() const {
    return !elseIfs_.empty();
}

void WhileLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void WhileLoopNode::print(std::ostream& stream) const {
    stream << "WHILE" << this->getCondition() << "DO" << this->getStatements() << "END";
}

bool WhileLoopNode::hasExit() {
    bool res = LoopNode::hasExit();
    for (size_t i = 0; i < elseIfs_.size(); ++i) {
        res = res || elseIfs_.at(i)->getStatements()->hasExit();
    }
    return res;
}

bool WhileLoopNode::isReturn() {
    bool res = LoopNode::isReturn();
    for (size_t i = 0; i < elseIfs_.size(); ++i) {
        res = res && elseIfs_.at(i)->getStatements()->isReturn();
    }
    return res;
}


void RepeatLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void RepeatLoopNode::print(std::ostream& stream) const {
    stream << "REPEAT" << this->getStatements() << "UNTIL" << this->getCondition();
}


QualifiedExpression* ForLoopNode::getCounter() const {
    return counter_.get();
}

ExpressionNode* ForLoopNode::getLow() const {
    return low_.get();
}

ExpressionNode* ForLoopNode::getHigh() const {
    return high_.get();
}

ExpressionNode * ForLoopNode::getStep() const {
    return step_.get();
}

void ForLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ForLoopNode::print(std::ostream& stream) const {
    stream << "FOR" << *counter_ << " := " << *low_ << " TO " << *high_ << " BY " << *step_ << " DO" << this->getStatements() << "END";
}