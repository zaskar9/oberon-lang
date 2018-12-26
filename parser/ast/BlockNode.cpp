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

void BlockNode::addStatement(std::unique_ptr<Node> statement) {
    statements_.push_back(std::move(statement));
}