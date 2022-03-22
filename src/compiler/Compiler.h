//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_COMPILER_H
#define OBERON_LANG_COMPILER_H


#include <boost/filesystem.hpp>
#include "codegen/CodeGen.h"
#include "data/symtab/SymbolExporter.h"
#include "logging/Logger.h"

class Compiler {

private:
    Logger *logger_;
    CodeGen *codegen_;

public:
    explicit Compiler(Logger *logger, CodeGen *codegen) : logger_(logger), codegen_(codegen) {};
    ~Compiler() = default;

    void compile(boost::filesystem::path file);

};


#endif //OBERON_LANG_COMPILER_H
