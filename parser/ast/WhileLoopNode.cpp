/*
 * Header of the AST while loop node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "WhileLoopNode.h"
#include "NodeVisitor.h"

WhileLoopNode::WhileLoopNode(const FilePos pos, std::unique_ptr<ExpressionNode> condition) :
        StatementNode(NodeType::while_loop, pos), condition_(std::move(condition)), statements_(std::make_unique<StatementSequenceNode>(pos)) {
}

WhileLoopNode::~WhileLoopNode() = default;

ExpressionNode* WhileLoopNode::getCondition() {
    return condition_.get();
}

StatementSequenceNode* WhileLoopNode::getStatements() {
    return statements_.get();
}

void WhileLoopNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void WhileLoopNode::print(std::ostream& stream) const {
    stream << "WHILE " << *condition_ << " DO " << *statements_;
}
