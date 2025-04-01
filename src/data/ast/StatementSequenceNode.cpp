/*
 * AST node representing a statement sequence in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "StatementSequenceNode.h"
#include "NodeVisitor.h"

void StatementSequenceNode::updateState(size_t pos, StatementNode *statement) {
    exit_ = exit_ || (statement->hasExit() || statement->isReturn());
    return_ = return_ || statement->isReturn();
    if (!term_ && statement->isTerminator()) {
        term_ = true;
        termIdx_ = pos;
    }
}

void StatementSequenceNode::addStatement(std::unique_ptr<StatementNode> statement) {
    if (statement) {
        updateState(statements_.size(), statement.get());
        statements_.push_back(std::move(statement));
    }
}

void StatementSequenceNode::insertStatement(size_t pos, std::unique_ptr<StatementNode> statement) {
    updateState(pos, statement.get());
    statements_.insert(statements_.begin() + (long) pos, std::move(statement));
}

StatementNode* StatementSequenceNode::getStatement(size_t num) const {
    return statements_.at(num).get();
}

size_t StatementSequenceNode::getStatementCount() const {
    return statements_.size();
}

bool StatementSequenceNode::hasExit() const {
    return exit_;
}

bool StatementSequenceNode::isReturn() const {
    return return_;
}

bool StatementSequenceNode::hasTerminator() const {
    return term_;
}

size_t StatementSequenceNode::getTerminatorIndex() const {
    return termIdx_;
}

void StatementSequenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void StatementSequenceNode::print(std::ostream& stream) const {
    for (auto const& statement: statements_) {
        stream << *statement << std::endl;
    }
}


