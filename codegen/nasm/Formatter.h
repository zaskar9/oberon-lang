/*
 * Formatter for assembly in the NASM file format.
 *
 * Created by Michael Grossniklaus on 1/23/20.
 */

#ifndef OBERON0C_FORMATTER_H
#define OBERON0C_FORMATTER_H


#include "Assembly.h"
#include "Instruction.h"

class Formatter {

private:
    size_t colWidth_;
    const Assembly* nasm_;

    static std::unordered_map<OpCode, std::string> codes;

    void format(std::ostream &stream, const Section* section) const;
    void format(std::ostream &stream, const Instruction* instruction) const;
    void format(std::ostream &stream, const Operand* operand, OpMode mode) const;
    void title(std::ostream &stream, std::string title) const;

public:
    explicit Formatter(const Assembly* nasm) : colWidth_(9), nasm_(nasm) { };
    ~Formatter() = default;

    void format(std::ostream &stream) const;

};

#endif //OBERON0C_FORMATTER_H
