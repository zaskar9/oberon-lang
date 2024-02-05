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


ConditionalLoopNode::~ConditionalLoopNode() = default;

//void ConditionalLoopNode::setCondition(std::unique_ptr<ExpressionNode> condition) {
//    condition_ = std::move(condition);
//}

ExpressionNode* ConditionalLoopNode::getCondition() const {
    return condition_.get();
}


void WhileLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void WhileLoopNode::print(std::ostream& stream) const {
    stream << "WHILE" << this->getCondition() << "DO" << this->getStatements() << "END";
}


void RepeatLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void RepeatLoopNode::print(std::ostream& stream) const {
    stream << "REPEAT" << this->getStatements() << "UNTIL" << this->getCondition();
}


ValueReferenceNode* ForLoopNode::getCounter() const {
    return counter_.get();
}

void ForLoopNode::setLow(std::unique_ptr<ExpressionNode> low) {
    low_ = std::move(low);
}

ExpressionNode* ForLoopNode::getLow() const {
    return low_.get();
}

void ForLoopNode::setHigh(std::unique_ptr<ExpressionNode> high) {
    high_ = std::move(high);
}

ExpressionNode* ForLoopNode::getHigh() const {
    return high_.get();
}

void ForLoopNode::setStep(std::unique_ptr<ExpressionNode> step) {
    step_ = std::move(step);
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