/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#ifndef OBERON_LANG_LLVMCODEGEN_H
#define OBERON_LANG_LLVMCODEGEN_H


#include <csignal>
#include <string>
#include <filesystem>

#include <boost/predef.h>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>

#include "data/ast/ASTContext.h"
#include "codegen/CodeGen.h"
#include "logging/Logger.h"

int mingw_noop_main();

void ubsantrap_handler(uint16_t code);
[[noreturn]] void trap_handler(int, siginfo_t*, void*);
#if BOOST_ARCH_ARM || BOOST_ARCH_X86
void register_signal_handler();
#endif

using std::filesystem::path;
using std::string;

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

    void emit(llvm::Module *, path, OutputFileType);
    static std::string getLibName(const string &, bool, const llvm::Triple &);

public:
    LLVMCodeGen(CompilerConfig &);
    ~LLVMCodeGen() override = default;

    std::string getDescription() final;

    void configure() final;

    void generate(ASTContext *, path) final;
#ifndef _LLVM_LEGACY
    int jit(ASTContext *, path) final;
#endif

};


#endif //OBERON_LANG_LLVMCODEGEN_H
