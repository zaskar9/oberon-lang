//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_COMPILER_H
#define OBERON_LANG_COMPILER_H


#include "CompilerFlags.h"
#include "codegen/CodeGen.h"
#include "data/symtab/SymbolExporter.h"
#include "logging/Logger.h"
#include "system/OberonSystem.h"
#include <memory>
#include <boost/filesystem.hpp>

using std::unique_ptr;
using boost::filesystem::path;

class Compiler {

private:
    Logger *logger_;
    CompilerFlags *flags_;
    CodeGen *codegen_;
    unique_ptr<OberonSystem> system_;

    unique_ptr<Node> run(const path&);

public:
    explicit Compiler(Logger *logger, CompilerFlags *flags, CodeGen *codegen) :
            logger_(logger), flags_(flags), codegen_(codegen), system_(std::make_unique<Oberon07>()) {};
    ~Compiler() = default;

    void compile(const path&);
#ifndef _LLVM_LEGACY
    int jit(const path&);
#endif

};


#endif //OBERON_LANG_COMPILER_H
