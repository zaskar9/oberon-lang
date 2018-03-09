/*
 * Implementation of the AST code block nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "BlockNode.h"

BlockNode::BlockNode(NodeType nodeType, FilePos pos) :
        Node(nodeType, pos), constants_(), types_(), variables_(), procedures_(), statements_() {
}

BlockNode::~BlockNode() = default;

void BlockNode::addConstant(std::unique_ptr<const ConstantNode> constant) {
    constants_.push_back(std::move(constant));
}

void BlockNode::addType(const std::shared_ptr<const TypeNode> &type) {
    types_.push_back(type);
}

void BlockNode::addVariable(std::unique_ptr<const NamedValueNode> variable) {
    variables_.push_back(std::move(variable));
}

void BlockNode::addProcedure(std::unique_ptr<const ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

void BlockNode::addStatement(std::unique_ptr<const Node> statement) {
    statements_.push_back(std::move(statement));
}