/*
 * AST node representing a module in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "ModuleNode.h"
#include "NodeVisitor.h"

std::string ModuleNode::getName() const {
    return name_;
}

void ModuleNode::addProcedure(std::unique_ptr<ProcedureNode> procedure) {
    procedures_.push_back(std::move(procedure));
}

ProcedureNode* ModuleNode::getProcedure(size_t num) const {
    return procedures_.at(num).get();
}

size_t ModuleNode::getProcedureCount() const {
    return procedures_.size();
}


void ModuleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ModuleNode::print(std::ostream &stream) const {
    stream << "MODULE " << name_;
}