/*
 * Header of the basic block class used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#ifndef OBERON0C_BASICBLOCK_H
#define OBERON0C_BASICBLOCK_H


#include <string>

class BasicBlock {

private:
    std::string comment_;
    std::unique_ptr<Label> label_;
    std::vector<std::unique_ptr<Instruction>> instructions_;
    std::unique_ptr<BasicBlock> next_;

public:
    explicit BasicBlock(std::unique_ptr<Label> label, std::string &comment) :
        label_(std::move(label)), comment_(comment), instructions_(), next_() { };
    ~BasicBlock() = default;

    const Label* getLabel() const;
    const std::string getComment() const;

    void addInstruction(std::unique_ptr<Instruction> instruction);

};


#endif //OBERON0C_BASICBLOCK_H
