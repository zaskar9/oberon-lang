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
    statements_.push_back(std::move(statement));
}

void StatementSequenceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void StatementSequenceNode::print(std::ostream& stream) const {
    for (auto const& statement: statements_) {
        stream << *statement << std::endl;
    }
}


