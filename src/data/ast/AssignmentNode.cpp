/*
 * AST node representing an assignment in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#include "AssignmentNode.h"
#include "NodeVisitor.h"

ValueReferenceNode * AssignmentNode::getLvalue() {
    return lvalue_.get();
}

void AssignmentNode::setRvalue(std::unique_ptr<ExpressionNode> rvalue) {
    rvalue_ = std::move(rvalue);
}

ExpressionNode * AssignmentNode::getRvalue() {
    return rvalue_.get();
}

void AssignmentNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void AssignmentNode::print(std::ostream& stream) const {
    stream << *lvalue_ << " := " << *rvalue_;
}
