/*
 * Implementation of the NASM assembly produced code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "NasmAssembly.h"

BssSection* NasmAssembly::getBssSection() {
    return bss_.get();
}

DataSection* NasmAssembly::getDataSection() {
    return data_.get();
}

TextSection* NasmAssembly::getTextSection() {
    return text_.get();
}
