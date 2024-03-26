/*
 * AST node representing a module in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#include "ModuleNode.h"
#include "NodeVisitor.h"
#include <algorithm>

vector<unique_ptr<ImportNode>> &ModuleNode::imports() {
    return imports_;
}

void ModuleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ModuleNode::print(std::ostream &stream) const {
    stream << "MODULE " << *getIdentifier();
}