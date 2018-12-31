/*
 * Implementation of the AST module nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "ModuleNode.h"
#include "NodeVisitor.h"

ModuleNode::ModuleNode(const FilePos pos, const std::string &name) :
        BlockNode(NodeType::module, pos), name_(name), procedures_() {
}

ModuleNode::~ModuleNode() = default;

const std::string ModuleNode::getName() const {
    return name_;
}

void ModuleNode::addProcedure(std::unique_ptr<ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

void ModuleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ModuleNode::print(std::ostream &stream) const {
    stream << "MODULE " << name_;
}