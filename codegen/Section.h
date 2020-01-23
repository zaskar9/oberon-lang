/*
 * Header of the NASM assembly sections used code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_SECTION_H
#define OBERON0C_SECTION_H


#include <ostream>
#include <string>
#include <vector>
#include "Instruction.h"

class Section {

private:
    std::string name_;
    std::vector<std::unique_ptr<Instruction>> instructions_;

public:
    explicit Section(const std::string &name) : name_(name), instructions_() { };
    virtual ~Section() = default;

    const std::string getName() const;
    void addInstruction(std::unique_ptr<Instruction> instruction);

    friend std::ostream& operator<<(std::ostream &stream, const Section &section);

};

#endif //OBERON0C_SECTION_H
