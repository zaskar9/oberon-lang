/*
 * Implementation of the AST statement sequence node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "StatementSequenceNode.h"
#include "NodeVisitor.h"

StatementSequenceNode::StatementSequenceNode(const FilePos pos) : Node(NodeType::statement_sequence, pos), statements_() {
}

void StatementSequenceNode::addStatement(std::unique_ptr<StatementNode> statement) {
    if (statement != nullptr) {
        statements_.push_back(std::move(statement));
    }
}

StatementNode* StatementSequenceNode::getStatement(size_t num) const {
    return statements_.at(num).get();
}

size_t StatementSequenceNode::getStatementCount() const {
    return statements_.size();
}

void StatementSequenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void StatementSequenceNode::print(std::ostream& stream) const {
    for (auto const& statement: statements_) {
        stream << *statement << std::endl;
    }
}


