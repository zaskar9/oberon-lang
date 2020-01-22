/*
 * Implementation of the NASM assembly produced code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "NasmAssembly.h"

NasmAssembly::NasmAssembly() {
    bss_ = std::make_unique<BssSection>();
    data_ = std::make_unique<DataSection>();
    text_ = std::make_unique<TextSection>();
}

BssSection* NasmAssembly::getBssSection() const {
    return bss_.get();
}

DataSection* NasmAssembly::getDataSection() const {
    return data_.get();
}

TextSection* NasmAssembly::getTextSection() const {
    return text_.get();
}

std::ostream& operator<<(std::ostream &stream, const NasmAssembly &assembly) {
    std::string indent = std::string(9, ' ');
    stream << "; " << std::string(30, '-') <<  " BSS Section " << std::string(31, '-') << std::endl;
    stream << *assembly.getBssSection() << std::endl;
    stream << "; " << std::string(30, '-') <<  " Data Section " << std::string(30, '-') << std::endl;
    stream << *assembly.getDataSection() << std::endl;
    stream << "; " << std::string(30, '-') <<  " Text Section " << std::string(30, '-') << std::endl;
    stream << *assembly.getTextSection() << std::endl;
    return stream;
}
