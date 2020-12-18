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

StatementSequenceNode* IfThenElseNode::addThenStatements(const FilePos pos) {
    thenStatements_ = std::make_unique<StatementSequenceNode>(pos);
    return thenStatements_.get();
}

StatementSequenceNode* IfThenElseNode::getThenStatements() const {
    return thenStatements_.get();
}

StatementSequenceNode* IfThenElseNode::addElseIf(const FilePos pos, std::unique_ptr<ExpressionNode> condition) {
    auto elseIf = std::make_unique<ElseIfNode>(pos, std::move(condition));
    auto statements = elseIf->getStatements();
    elseIfs_.push_back(std::move(elseIf));
    return statements;
}

ElseIfNode* IfThenElseNode::getElseIf(size_t num) const {
    return elseIfs_.at(num).get();
}

size_t IfThenElseNode::getElseIfCount() const {
    return elseIfs_.size();
}

bool IfThenElseNode::hasElseIf() const {
    return elseIfs_.size() > 0;
}

StatementSequenceNode* IfThenElseNode::addElseStatements(const FilePos pos) {
    elseStatements_ = std::make_unique<StatementSequenceNode>(pos);
    return elseStatements_.get();
}

StatementSequenceNode* IfThenElseNode::getElseStatements() const {
    return elseStatements_.get();
}

bool IfThenElseNode::hasElse() const {
    return elseStatements_ != nullptr;
}

void IfThenElseNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void IfThenElseNode::print(std::ostream& stream) const {
    stream << "IF " << *condition_ << " THEN " << *thenStatements_;
    for (auto const& elseIf : elseIfs_) {
        stream << *elseIf;
    }
    if (elseStatements_ !=nullptr) {
        stream << " ELSE " << *elseStatements_;
    }
    stream << "END";
}