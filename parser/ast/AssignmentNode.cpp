/*
 * Implementation of the AST assignment node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "AssignmentNode.h"

AssignmentNode::AssignmentNode(const FilePos pos, std::unique_ptr<NamedValueReferenceNode> lvalue,
        std::unique_ptr<ExpressionNode> rvalue) : StatementNode(NodeType::assignment, pos),
        lvalue_(std::move(lvalue)), rvalue_(std::move(rvalue)) {
}

AssignmentNode::~AssignmentNode() = default;

NamedValueReferenceNode* AssignmentNode::getLvalue() {
    return lvalue_.get();
}

ExpressionNode* AssignmentNode::getRvalue() {
    return rvalue_.get();
}

void AssignmentNode::print(std::ostream& stream) const {
    stream << *lvalue_ << " := " << *rvalue_;
}
