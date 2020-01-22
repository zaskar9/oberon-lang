/*
 * Header of the NASM assembly produced code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#ifndef OBERON0C_NASMASSEMBLY_H
#define OBERON0C_NASMASSEMBLY_H


#include <memory>
#include <vector>
#include "Section.h"

class NasmAssembly {

private:
    std::unique_ptr<BssSection> bss_;
    std::unique_ptr<DataSection> data_;
    std::unique_ptr<TextSection> text_;

public:
    explicit NasmAssembly();
    ~NasmAssembly() = default;

    BssSection* getBssSection();
    DataSection* getDataSection();
    TextSection* getTextSection();

};

#endif //OBERON0C_NASMASSEMBLY_H
