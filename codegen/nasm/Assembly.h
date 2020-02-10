/*
 * Assembly in the NASM file format used by code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_ASSEMBLY_H
#define OBERON0C_ASSEMBLY_H


#include <memory>
#include <ostream>
#include <vector>
#include <unordered_map>
#include "Instruction.h"
#include "Operand.h"


class Context {

private:
    std::unordered_map<std::string, std::unique_ptr<Label>> labels_;

public:
    const Label* putLabel(std::string name);
    const Label* getLabel(std::string name) const;
};


class Section {

private:
    std::string name_;
    Context* context_;
    std::vector<std::unique_ptr<Instruction>> instructions_;

public:
    explicit Section(const std::string &name, Context* context) : name_(name), context_(context), instructions_() { };
    virtual ~Section() = default;

    const std::string getName() const;
    void addInstruction(std::string label, std::unique_ptr<Instruction> instruction);
    void addInstruction(std::unique_ptr<Instruction> instruction);
    const Instruction* getInstruction(size_t i) const;
    size_t size() const;
};


class Assembly {

private:
    std::unique_ptr<Section> bss_;
    std::unique_ptr<Section> data_;
    std::unique_ptr<Section> text_;
    std::unique_ptr<Context> context_;

public:
    explicit Assembly();
    ~Assembly() = default;

    Section* getBssSection() const;
    Section* getDataSection() const;
    Section* getTextSection() const;

    const Label* getLabel(std::string name) const;

};

#endif //OBERON0C_ASSEMBLY_H
