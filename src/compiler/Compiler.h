//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_COMPILER_H
#define OBERON_LANG_COMPILER_H


#include <memory>
#include <boost/filesystem.hpp>

#include "CompilerFlags.h"
#include "codegen/CodeGen.h"
#include "logging/Logger.h"
#include "system/OberonSystem.h"
#include "data/ast/ASTContext.h"

using std::unique_ptr;
using boost::filesystem::path;

class Compiler {

private:
    CompilerFlags *flags_;
    Logger *logger_;
    CodeGen *codegen_;
    unique_ptr<OberonSystem> system_;

    unique_ptr<ASTContext> run(const path&);

public:
    explicit Compiler(CompilerFlags *flags, Logger *logger, CodeGen *codegen) :
            flags_(flags), logger_(logger), codegen_(codegen), system_(std::make_unique<Oberon07>()) {};
    ~Compiler() = default;

    void compile(const path&);
#ifndef _LLVM_LEGACY
    int jit(const path&);
#endif

};


#endif //OBERON_LANG_COMPILER_H
