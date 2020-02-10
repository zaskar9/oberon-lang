/*
 * Formatter for assembly in the NASM file format.
 *
 * Created by Michael Grossniklaus on 1/23/20.
 */


#include <iomanip>
#include "Formatter.h"

void Formatter::format(std::ostream &stream) const {
    title(stream, "BSS Section");
    format(stream, nasm_->getBssSection());
    title(stream, "Data Section");
    format(stream, nasm_->getDataSection());
    title(stream, "Text Section");
    format(stream, nasm_->getTextSection());
}

void Formatter::format(std::ostream &stream, const Section* section) const {
    stream << std::setw(colWidth_) << "" << std::setw(colWidth_) << std::left << "section" << section->getName() << std::endl;
    for (size_t i = 0; i < section->size(); i++) {
        format(stream, section->getInstruction(i));
    }
    stream << std::endl;
}

void Formatter::format(std::ostream &stream, const Instruction *instruction) const {
    auto label = instruction->getLabel();
    stream << std::setw(colWidth_) << std::left;
    if (label != nullptr) {
        std::string name = label->getName() + ": ";
        stream << name;
        if (name.size() > colWidth_) {
            stream << std::endl << std::setw(colWidth_) << "";
        }
    } else {
        stream << "";
    }
    std::string code = codes[instruction->getCode()];
    stream << code;
    auto mode = instruction->getMode();
    if (code.size() + 2 < colWidth_) {
        stream << std::string(colWidth_ - code.size() - 1, ' ');
    } else {
        stream << ' ';
    }
    auto fst = instruction->getFirst();
    if (fst != nullptr) {
        format(stream, fst, mode);
        stream << ", ";
    }
    auto snd = instruction->getSecond();
    if (snd != nullptr) {
        format(stream, snd, mode);
    }
    stream << std::endl;
}

void Formatter::format(std::ostream &stream, const Operand *operand, OpMode mode) const {
    if (auto label = dynamic_cast<const Label*>(operand)) {
        stream << label->getName();
    } else if (auto reg = dynamic_cast<const Register*>(operand)) {
        stream << reg->getName(mode);
    } else if (auto imm = dynamic_cast<const Immediate*>(operand)) {
        stream << imm->getValue();
    } else if (auto eaddr = dynamic_cast<const EffectiveAddress*>(operand)) {
        stream << "[" << eaddr->getBase()->getName();
        int displacement = eaddr->getDisplacement();
        if (displacement > 0) {
            stream << "+" << displacement;
        } else if (displacement < 0) {
            stream << displacement;
        }
        auto index = eaddr->getIndex();
        if (index != nullptr) {
            stream << index->getName();
            int scale = eaddr->getScale();
            if (scale > 0) {
                stream << "*" << scale;
            }
        }
        stream << "]";
    } else if (auto laddr = dynamic_cast<const LabelAddress*>(operand)) {
        stream << "[rel " << laddr->getLabel()->getName() << "]";
    }
}

void Formatter::title(std::ostream &stream, std::string title) const {
    int lineWidth = 9 * colWidth_;
    std::string line = std::string(lineWidth, '=');
    std::string padded(" " + title + " ");
    line.replace(lineWidth / 2 - (padded.length() - 1) / 2, padded.length(), padded);
    stream << "; " << line << std::endl;
}

std::unordered_map<OpCode, std::string> Formatter::codes = {
        { OpCode::push, "push" }, { OpCode::pop, "pop" }, { OpCode::mov, "mov" },
        { OpCode::lea, "lea" },  { OpCode::add, "add" }, { OpCode::sub, "sub" },
        { OpCode::imul, "imul" }, { OpCode::idiv, "idiv" },
        { OpCode::inc, "inc" }, { OpCode::dec, "dec" }, { OpCode::cmp, "cmp" },
        { OpCode::je, "je" }, { OpCode::jne, "jne" }, { OpCode::jl, "jl" },
        { OpCode::jle, "jle" }, { OpCode::jg, "jg" }, { OpCode::jge, "jge" },
        { OpCode::call, "call" }, { OpCode::ret, "ret" }, { OpCode::res,  "res" },
        { OpCode::nop, "nop" }
};
