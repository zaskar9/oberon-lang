/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#ifndef OBERON_LANG_LLVMCODEGEN_H
#define OBERON_LANG_LLVMCODEGEN_H


#include "analyzer/Analyzer.h"
#include "codegen/CodeGen.h"
#include "logging/Logger.h"
#include <string>
#include <boost/filesystem.hpp>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>


int mingw_noop_main(void);

class LLVMCodeGen final : public CodeGen {

private:
    CompilerFlags *flags_;
    Logger *logger_;
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
    LLVMCodeGen(CompilerFlags *flags, Logger *logger);
    ~LLVMCodeGen() override = default;

    std::string getDescription() final;

    void configure(CompilerFlags *flags) final;

    void generate(Node *ast, boost::filesystem::path path) final;
#ifndef _LLVM_LEGACY
    int jit(Node *ast, boost::filesystem::path path) final;
#endif

};


#endif //OBERON_LANG_LLVMCODEGEN_H
