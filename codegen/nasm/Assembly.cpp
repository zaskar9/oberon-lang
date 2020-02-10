/*
 * Assembly in the NASM file format used by code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "Assembly.h"

const Label* Context::putLabel(std::string name) {
    auto it = labels_.insert(std::make_pair(name, std::make_unique<Label>(name)));
    return it.first->second.get();
}

const Label* Context::getLabel(std::string name) const {
    return labels_.find(name)->second.get();
}

const std::string Section::getName() const {
    return name_;
}

void Section::addInstruction(std::string name, std::unique_ptr<Instruction> instruction) {
    auto label = context_->putLabel(name);
    instruction->setLabel(label);
    instructions_.push_back(std::move(instruction));
}

void Section::addInstruction(std::unique_ptr<Instruction> instruction) {
    instructions_.push_back(std::move(instruction));
}

const Instruction* Section::getInstruction(size_t i) const {
    return instructions_[i].get();
}

size_t Section::size() const {
    return instructions_.size();
}

Assembly::Assembly() {
    context_ = std::make_unique<Context>();
    bss_ = std::make_unique<Section>(".bss", context_.get());
    data_ = std::make_unique<Section>(".data", context_.get());
    text_ = std::make_unique<Section>(".text", context_.get());
}

Section* Assembly::getBssSection() const {
    return bss_.get();
}

Section* Assembly::getDataSection() const {
    return data_.get();
}

Section* Assembly::getTextSection() const {
    return text_.get();
}

const Label* Assembly::getLabel(std::string name) const {
    return context_->getLabel(name);
}


