/*
 * Instructions used in the NASM file format.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#include "Instruction.h"


void Instruction::setLabel(const Label *label) {
    label_ = label;
}
const Label* Instruction::getLabel() const {
    return label_;
}

OpCode Instruction::getCode() const {
    return code_;
}

OpMode Instruction::getMode() const {
    return mode_;
}

const Operand* Instruction::getFirst() const {
    return fst_;
}

void Instruction::setComment(std::string comment) {
    comment_ = comment;
}

const std::string Instruction::getComment() const {
    return comment_;
}

const Register* RegisterInstruction::getSecond() const {
    return snd_;
}

const Immediate* ImmediateInstruction::getSecond() const {
    return snd_.get();
}

const Operand* AddressInstruction::getFirst() const {
    if (swap_) {
        return addr_.get();
    }
    return Instruction::getFirst();
}

const Operand * AddressInstruction::getSecond() const {
    if (swap_) {
        return Instruction::getFirst();
    }
    return addr_.get();
}
