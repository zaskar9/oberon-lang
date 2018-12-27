/*
 * Implementation of the AST code block nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "BlockNode.h"

BlockNode::BlockNode(const NodeType nodeType, const FilePos pos) : Node(nodeType, pos),
        constants_(), types_(), variables_(), procedures_(), statements_(std::make_unique<StatementSequenceNode>(pos)) {
}

BlockNode::~BlockNode() = default;

void BlockNode::addConstant(std::unique_ptr<ConstantNode> constant) {
    constants_.push_back(std::move(constant));
}

void BlockNode::addType(std::unique_ptr<TypeNode> type) {
    types_.push_back(std::move(type));
}

void BlockNode::addVariable(std::unique_ptr<VariableNode> variable) {
    variables_.push_back(std::move(variable));
}

void BlockNode::addProcedure(std::shared_ptr<ProcedureNode> procedure) {
    procedures_.push_back(procedure);
}

StatementSequenceNode* BlockNode::getStatements() {
    return statements_.get();
}

void BlockNode::print(std::ostream& stream) const {
    for (auto const& constant: constants_) {
        stream << *constant << std::endl;
    }
    for (auto const& type: types_) {
        stream << *type << std::endl;
    }
    for (auto const& variable: variables_) {
        stream << *variable << std::endl;
    }
    for (auto const& procedure: procedures_) {
        stream << procedure << std::endl;
    }
    stream << *statements_;
}