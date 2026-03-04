//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_COMPILER_H
#define OBERON_LANG_COMPILER_H


#include <filesystem>
#include <memory>

#include "Logger.h"
#include "CompilerConfig.h"
#include "codegen/CodeGen.h"
#include "system/OberonSystem.h"
#include "data/ast/ASTContext.h"

using std::unique_ptr;
using std::filesystem::path;

class Compiler {

public:
    explicit Compiler(CompilerConfig &config, CodeGen *codegen) :
            config_(config), logger_(config.logger()), codegen_(codegen), system_(std::make_unique<Oberon07>()) {};
    ~Compiler() = default;

    void compile(const path&);
#ifndef _LLVM_LEGACY
    int jit(const path&);
#endif

private:
    CompilerConfig &config_;
    Logger &logger_;
    CodeGen *codegen_;
    unique_ptr<OberonSystem> system_;

    unique_ptr<ASTContext> run(const path&);

};


#endif //OBERON_LANG_COMPILER_H
