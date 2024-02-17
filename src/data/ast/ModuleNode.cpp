/*
 * AST node representing a module in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "ModuleNode.h"
#include "NodeVisitor.h"
#include <algorithm>

void ModuleNode::setAlias(std::string alias) {
    alias_ = alias;
}

std::string ModuleNode::getAlias() const {
    return alias_;
}

void ModuleNode::addImport(std::unique_ptr<ImportNode> import) {
    imports_.push_back(std::move(import));
}

ImportNode* ModuleNode::getImport(size_t num) const {
    return imports_.at(num).get();
}

size_t ModuleNode::getImportCount() const {
    return imports_.size();
}

void ModuleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ModuleNode::print(std::ostream &stream) const {
    stream << "MODULE " << *getIdentifier();
}