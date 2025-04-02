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

// Signal handling only supported on POSIX platforms
#if (defined(_WIN32) || defined(_WIN64)) || !defined(__MINGW32__)
#include <windows.h>
#undef ERROR
#undef max
#undef min
#define WINDOWS_TRAPS
#else
#include <unistd.h>
#define POSIX_TRAPS
#endif

int mingw_noop_main();

void ubsantrap_handler(uint16_t code);
#if defined(POSIX_TRAPS)
[[noreturn]] void trap_handler(int, siginfo_t*, void*);
#elif defined(WINDOWS_TRAPS)
LONG WINAPI trap_handler(EXCEPTION_POINTERS* info);
#endif
void register_signal_handler();

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
