//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_COMPILER_H
#define OBERON_LANG_COMPILER_H


#include "codegen/CodeGen.h"
#include "data/symtab/SymbolExporter.h"
#include "logging/Logger.h"
#include "CompilerFlags.h"
#include <boost/filesystem.hpp>

class Compiler {

private:
    Logger *logger_;
    CompilerFlags *flags_;
    CodeGen *codegen_;

public:
    explicit Compiler(Logger *logger, CompilerFlags *flags, CodeGen *codegen) : logger_(logger), flags_(flags), codegen_(codegen) {};
    ~Compiler() = default;

    void compile(boost::filesystem::path file);

};


#endif //OBERON_LANG_COMPILER_H
