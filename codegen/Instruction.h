/*
 * Header of the instruction class used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#ifndef OBERON0C_INSTRUCTION_H
#define OBERON0C_INSTRUCTION_H


#include <string>
#include <vector>
#include "Operand.h"

enum class OpCode : char {
   push, pop, mov, lea,
   add, sub, inc, dec,
   exor,
   cmp,
   jl, jle, jg, jge,
   call, ret
};

enum class OpMode : char {
   q64, d32, w16, b8
};

class Instruction {

private:
   std::unique_ptr<Label> label_;
   OpCode code_;
   OpMode mode_;
   std::vector<const Operand*> operands_;
   std::string comment_;

public:
   explicit Instruction(OpCode code, const Operand* operands...) :
         label_(), code_(code), mode_(), operands_ {operands }, comment_() { };
   explicit Instruction(OpCode code, OpMode mode, const Operand* operands...) :
         label_(), code_(code), mode_(mode), operands_ {operands }, comment_() { };
   ~Instruction() = default;

   Label* setLabel(std::string &label);
   void setComment(std::string &comment);
};


#endif //OBERON0C_INSTRUCTION_H
