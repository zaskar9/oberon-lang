/*
 * Implementation of the AST loop nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 01/14/19.
 */

#include "LoopNode.h"
#include "NodeVisitor.h"

LoopNode::LoopNode(const NodeType type, const FilePos pos) :
        StatementNode(type, pos), statements_(std::make_unique<StatementSequenceNode>(pos)) {
}

LoopNode::LoopNode(const FilePos pos) : LoopNode(NodeType::loop, pos) {
}

LoopNode::~LoopNode() = default;

StatementSequenceNode* LoopNode::getStatements() const {
    return statements_.get();
}

void LoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void LoopNode::print(std::ostream& stream) const {
    stream << "LOOP" << *statements_ << "END";
}

ConditionalLoopNode::ConditionalLoopNode(const NodeType type, const FilePos pos, std::unique_ptr<ExpressionNode> condition) :
        LoopNode(type, pos), condition_(std::move(condition)) {
}

ConditionalLoopNode::ConditionalLoopNode(const NodeType type, const FilePos pos) : ConditionalLoopNode(type, pos, nullptr) {
}

ConditionalLoopNode::~ConditionalLoopNode() = default;

void ConditionalLoopNode::setCondition(std::unique_ptr<ExpressionNode> condition) {
    condition_ = std::move(condition);
}

ExpressionNode* ConditionalLoopNode::getCondition() const {
    return condition_.get();
}

WhileLoopNode::WhileLoopNode(const FilePos pos, std::unique_ptr<ExpressionNode> condition) :
        ConditionalLoopNode(NodeType::while_loop, pos, std::move(condition)) {
}

WhileLoopNode::~WhileLoopNode() = default;

void WhileLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void WhileLoopNode::print(std::ostream& stream) const {
    stream << "WHILE" << this->getCondition() << "DO" << this->getStatements() << "END";
}

RepeatLoopNode::RepeatLoopNode(const FilePos pos) : ConditionalLoopNode(NodeType::repeat_loop, pos) {
}

RepeatLoopNode::~RepeatLoopNode() = default;

void RepeatLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void RepeatLoopNode::print(std::ostream& stream) const {
    stream << "REPEAT" << this->getStatements() << "UNTIL" << this->getCondition();
}

ForLoopNode::ForLoopNode(const FilePos pos, std::unique_ptr<NamedValueReferenceNode> counter,
        std::unique_ptr<ExpressionNode> low, std::unique_ptr<ExpressionNode> high, int step) :
        LoopNode(NodeType::for_loop, pos), counter_(std::move(counter)), low_(std::move(low)), high_(std::move(high)), step_(step) {
}

ForLoopNode::~ForLoopNode() = default;

NamedValueReferenceNode* ForLoopNode::getCounter() const {
    return counter_.get();
}

ExpressionNode* ForLoopNode::getLow() const {
    return low_.get();
}

ExpressionNode* ForLoopNode::getHigh() const {
    return high_.get();
}

int ForLoopNode::getStep() const {
    return step_;
}

void ForLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ForLoopNode::print(std::ostream& stream) const {
    stream << "FOR" << *counter_ << " := " << *low_ << " TO " << *high_ << " BY " << step_ << " DO" << this->getStatements() << "END";
}