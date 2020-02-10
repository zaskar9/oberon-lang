/*
 * Instructions used in the NASM file format.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#ifndef OBERON0C_INSTRUCTION_H
#define OBERON0C_INSTRUCTION_H


#include <string>
#include <vector>
#include "Operand.h"

enum class OpCode : unsigned char {
   push, pop, mov, lea,
   add, sub, imul, idiv, inc, dec,
   exor,
   cmp,
   je, jne, jl, jle, jg, jge,
   call, ret,
   res,
   nop
};

class Instruction {

private:
    const Label* label_;
    OpCode code_;
    OpMode mode_;
    const Register* fst_;
    std::string comment_;

public:
    explicit Instruction(OpCode code, OpMode mode, const Register* fst) :
            label_(), code_(code), mode_(mode), fst_(fst), comment_() { };
    virtual ~Instruction() = default;

    void setLabel(const Label* label);
    const Label* getLabel() const;
    OpCode getCode() const;
    OpMode getMode() const;
    virtual const Operand* getFirst() const;
    virtual const Operand* getSecond() const = 0;
    void setComment(std::string comment);
    const std::string getComment() const;
};

class RegisterInstruction final : public Instruction {

private:
    const Register* snd_;

public:
    explicit RegisterInstruction(OpCode code, OpMode mode, const Register* fst, const Register* snd) :
            Instruction(code, mode, fst), snd_(snd) { };
    virtual ~RegisterInstruction() override = default;

    virtual const Register* getSecond() const override;

};

class ImmediateInstruction final : public Instruction {

private:
    std::unique_ptr<Immediate> snd_;

public:
    explicit ImmediateInstruction(OpCode code, OpMode mode, const Register* fst, int val) :
            Instruction(code, mode, fst), snd_(std::make_unique<Immediate>(val)) { };
    virtual ~ImmediateInstruction() override = default;

    virtual const Immediate* getSecond() const override;

};

class AddressInstruction final : public Instruction {

private:
    std::unique_ptr<Address> addr_;
    bool swap_;

public:
    explicit AddressInstruction(OpCode code, OpMode mode, const Register* reg, std::unique_ptr<Address> addr, bool swap) :
            Instruction(code, mode, reg), addr_(std::move(addr)), swap_(swap) { };

    virtual const Operand* getFirst() const override;
    virtual const Operand* getSecond() const override;
};


#endif //OBERON0C_INSTRUCTION_H
