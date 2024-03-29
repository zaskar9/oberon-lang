//
// Created by Michael Grossniklaus on 3/6/22.
//

#include "ImportNode.h"
#include "NodeVisitor.h"

Ident* ImportNode::getAlias() const {
    return alias_.get();
}

Ident* ImportNode::getModule() const {
    return module_.get();
}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

void ImportNode::print(std::ostream &stream) const {
    stream << (alias_ ? "" : to_string(*alias_) + " := ") << to_string(*module_);
}