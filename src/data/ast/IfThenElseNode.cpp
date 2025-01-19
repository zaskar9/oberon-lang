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
