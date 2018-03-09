/*
 * Implementation of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#include "ConstantNode.h"

ConstantNode::ConstantNode(const NodeType nodeType, const FilePos pos, const std::string &name,
                           const std::shared_ptr<const BasicTypeNode> &type, std::unique_ptr<const ValueNode> value) :
        NamedValueNode(nodeType, pos, name, type), value_(std::move(value)) {
}

ConstantNode::~ConstantNode() = default;

const ValueNode* ConstantNode::getValue() const {
    return value_.get();
}

void ConstantNode::print(std::ostream &stream) const {
    stream << "CONST " << this->getName() << " = " << *value_;
}
