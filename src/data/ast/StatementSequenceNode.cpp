/*
 * AST node representing a statement sequence in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "StatementSequenceNode.h"
#include "NodeVisitor.h"

void StatementSequenceNode::addStatement(std::unique_ptr<StatementNode> statement) {
    if (statement) {
        statements_.push_back(std::move(statement));
    }
}

void StatementSequenceNode::insertStatement(size_t pos, std::unique_ptr<StatementNode> statement) {
    statements_.insert(statements_.begin() + (long) pos, std::move(statement));
}

StatementNode* StatementSequenceNode::getStatement(size_t num) const {
    return statements_.at(num).get();
}

size_t StatementSequenceNode::getStatementCount() const {
    return statements_.size();
}

bool StatementSequenceNode::hasExit() {
    return exit_;
}

bool StatementSequenceNode::isReturn() {
    return return_;
}

size_t StatementSequenceNode::getReturnIndex() {
    return retIdx_;
}

void StatementSequenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void StatementSequenceNode::print(std::ostream& stream) const {
    for (auto const& statement: statements_) {
        stream << *statement << std::endl;
    }
}


