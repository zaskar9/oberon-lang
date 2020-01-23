/*
 * Implementation of the NASM assembly sections used code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "Section.h"

const std::string Section::getName() const {
    return name_;
}

void Section::addInstruction(std::unique_ptr<Instruction> instruction) {
    instructions_.push_back(std::move(instruction));
}

std::ostream& operator<<(std::ostream &stream, const Section &section) {
    std::string indent = std::string(9, ' ');
    stream << indent << "section  " << section.getName() << std::endl;
    return stream;
}