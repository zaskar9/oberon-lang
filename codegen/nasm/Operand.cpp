/*
 * Operands used by the instructions in the NASM file format.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */


#include "Operand.h"

const std::string Label::getName() const {
    return name_;
}

int Register::getId() const {
    return id_;
}

const std::string Register::getName() const {
    return this->getName(OpMode::q64);
}

const std::string Register::getName(OpMode mode) const {
    return names_.find(mode)->second;
}

int Immediate::getValue() const {
    return value_;
}

const Register* EffectiveAddress::getBase() const {
    return base_;
}

int EffectiveAddress::getDisplacement() const {
    return displacement_;
}

const Register* EffectiveAddress::getIndex() const {
    return index_;
}

int EffectiveAddress::getScale() const {
    return scale_;
}

const Label* LabelAddress::getLabel() const {
    return label_;
}