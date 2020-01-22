/*
 * Header of the basic block class used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#ifndef OBERON0C_BASICBLOCK_H
#define OBERON0C_BASICBLOCK_H


#include <memory>
#include <string>
#include <vector>
#include "Instruction.h"

class BasicBlock {

private:
    std::unique_ptr<Label> label_;
    std::string comment_;
    std::vector<std::unique_ptr<Instruction>> instructions_;

public:
    explicit BasicBlock(std::string label, std::string comment);
    ~BasicBlock() = default;

    const Label* getLabel() const;
    const std::string getComment() const;

    void addInstruction(std::unique_ptr<Instruction> instruction);

};


#endif //OBERON0C_BASICBLOCK_H