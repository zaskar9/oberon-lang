/*
 * Implementation of the NASM assembly sections used code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "Section.h"

void Section::setComment(const std::string& comment) {
    comment_ = comment;
}

const std::string Section::getComment() {
    return comment_;
}

Label* BssSection::reserveBytes(const std::string& name, int num) {
    auto label = std::make_unique<Label>(name);
    auto ptr = label.get();
    labels_[std::move(label)] = num;
    return ptr; // null?
}

BasicBlock* TextSection::addBasicBlock(const std::string& name, const std::string& comment) {
    auto block = std::make_unique<BasicBlock>(name, comment);
    blocks_.push_back(std::move(block));
    return blocks_.back().get();
}
