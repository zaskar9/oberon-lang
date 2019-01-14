/*
 * Implementation of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "ConstantNode.h"
#include "NodeVisitor.h"

ConstantNode::ConstantNode(const FilePos pos, const std::string &name, std::unique_ptr<ValueNode> value, int level) :
        NamedValueNode(NodeType::constant, pos, name, value->getType(), level, -1), value_(std::move(value)) {
}

ConstantNode::~ConstantNode() = default;

ValueNode* ConstantNode::getValue() const {
    return value_.get();
}

void ConstantNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ConstantNode::print(std::ostream &stream) const {
    stream << "CONST " << this->getName() << " = " << *value_ << ";";
}
