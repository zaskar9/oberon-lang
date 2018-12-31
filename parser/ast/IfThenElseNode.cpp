/*
 * Implementation of the AST if-then-else node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "IfThenElseNode.h"
#include "NodeVisitor.h"

ElseIf::ElseIf(const FilePos pos, std::unique_ptr<ExpressionNode> condition) :
        condition_(std::move(condition)), statements_(std::make_unique<StatementSequenceNode>(pos)) {
}

ExpressionNode* ElseIf::getCondition() {
    return condition_.get();
}

StatementSequenceNode* ElseIf::getStatements() {
    return statements_.get();
}

IfThenElseNode::IfThenElseNode(const FilePos pos, std::unique_ptr<ExpressionNode> condition) :
        StatementNode(NodeType::if_then_else, pos), condition_(std::move(condition)), elseIfStatements_() {
}

IfThenElseNode::~IfThenElseNode() = default;

ExpressionNode* IfThenElseNode::getCondition() {
    return condition_.get();
}

StatementSequenceNode* IfThenElseNode::addThenStatements(const FilePos pos) {
    thenStatements_ = std::make_unique<StatementSequenceNode>(pos);
    return thenStatements_.get();
}

StatementSequenceNode* IfThenElseNode::addElseIfStatements(const FilePos pos,
        std::unique_ptr<ExpressionNode> condition) {
    auto elseIf = std::make_unique<ElseIf>(pos, std::move(condition));
    auto statements = elseIf->getStatements();
    elseIfStatements_.push_back(std::move(elseIf));
    return statements;
}

StatementSequenceNode* IfThenElseNode::addElseStatements(const FilePos pos) {
    elseStatements_ = std::make_unique<StatementSequenceNode>(pos);
    return elseStatements_.get();
}

void IfThenElseNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void IfThenElseNode::print(std::ostream& stream) const {
    stream << "IF " << condition_ << " THEN " << thenStatements_ << " ELSE " << elseStatements_;
}
