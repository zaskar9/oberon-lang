/*
 * Header of the instruction class used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#ifndef OBERON0C_INSTRUCTION_H
#define OBERON0C_INSTRUCTION_H


enum class OpCode : char {
    push, pop, mov, lea, add, sub, jl, jg
};

class Instruction {

private:

};


#endif //OBERON0C_INSTRUCTION_H
