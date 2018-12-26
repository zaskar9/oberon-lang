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

void BlockNode::addConstant(std::shared_ptr<ConstantNode> constant) {
    constants_.push_back(std::move(constant));
}

void BlockNode::addType(std::shared_ptr<TypeNode> type) {
    types_.push_back(type);
}

void BlockNode::addVariable(std::shared_ptr<NamedValueNode> variable) {
    variables_.push_back(std::move(variable));
}

void BlockNode::addProcedure(std::shared_ptr<ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

void BlockNode::addStatement(std::shared_ptr<Node> statement) {
    statements_.push_back(std::move(statement));
}