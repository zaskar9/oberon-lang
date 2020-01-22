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

std::ostream& operator<<(std::ostream &stream, const DataSection &section) {
    std::string indent = std::string(9, ' ');
    stream << indent << "section  .data" << std::endl;
    return stream;
}

Label* BssSection::reserveBytes(const std::string& name, int num) {
    auto label = std::make_unique<Label>(name);
    auto ptr = label.get();
    auto size = labels_.size();
    labels_[std::move(label)] = num;
    return ptr; // null?
}

std::ostream& operator<<(std::ostream &stream, const BssSection &section) {
    std::string indent = std::string(9, ' ');
    stream << indent << "section  .bss" << std::endl;
    return stream;
}

BasicBlock* TextSection::addBasicBlock(const std::string& name, const std::string& comment) {
    auto block = std::make_unique<BasicBlock>(name, comment);
    blocks_.push_back(std::move(block));
    return blocks_.back().get();
}

std::ostream& operator<<(std::ostream &stream, const TextSection &section) {
    std::string indent = std::string(9, ' ');
    stream << indent << "section  .text" << std::endl;
    return stream;
}