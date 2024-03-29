/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#ifndef OBERON_LANG_LLVMCODEGEN_H
#define OBERON_LANG_LLVMCODEGEN_H


#include <string>
#include <boost/filesystem.hpp>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>

#include "data/ast/ASTContext.h"
#include "codegen/CodeGen.h"
#include "logging/Logger.h"

int mingw_noop_main(void);

class LLVMCodeGen final : public CodeGen {

private:
    CompilerConfig &config_;
    Logger &logger_;
    OutputFileType type_;
    llvm::LLVMContext ctx_;
    llvm::PassBuilder pb_;
    llvm::OptimizationLevel lvl_;
    llvm::TargetMachine *tm_;
    std::unique_ptr<llvm::orc::LLJIT> jit_;
    llvm::ExitOnError exitOnErr_;

    void emit(llvm::Module *module, boost::filesystem::path path, OutputFileType type);
    static std::string getLibName(const std::string &name, bool dylib, const llvm::Triple &triple);

public:
    LLVMCodeGen(CompilerConfig &config);
    ~LLVMCodeGen() override = default;

    std::string getDescription() final;

    void configure() final;

    void generate(ASTContext *ast, boost::filesystem::path path) final;
#ifndef _LLVM_LEGACY
    int jit(ASTContext *ast, boost::filesystem::path path) final;
#endif

};


#endif //OBERON_LANG_LLVMCODEGEN_H
