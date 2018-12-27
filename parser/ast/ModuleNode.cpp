/*
 * Implementation of the AST module nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "ModuleNode.h"

ModuleNode::ModuleNode(const FilePos pos, const std::string &name) : BlockNode(NodeType::module, pos), name_(name) {
}

ModuleNode::~ModuleNode() = default;

const std::string ModuleNode::getName() const {
    return name_;
}

void ModuleNode::print(std::ostream &stream) const {
    stream << "MODULE " << name_ << std::endl;
    BlockNode::print(stream);
}