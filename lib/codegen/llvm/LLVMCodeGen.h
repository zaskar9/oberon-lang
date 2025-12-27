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
#include <memory>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Target/TargetMachine.h>

#include "Logger.h"
#include "data/ast/ASTContext.h"
#include "codegen/CodeGen.h"

#ifdef _WINAPI
#include <windows.h>
#undef ERROR
#undef max
#undef min
#else
#include <unistd.h>
#endif

using llvm::Module;
using llvm::Triple;
using llvm::orc::LLJIT;
using std::string;
using std::unique_ptr;
using std::filesystem::path;

int mingw_noop_main();

[[noreturn]] void ubsantrap_handler(uint16_t code);
uint16_t decode_trap(void *);
#ifdef _WINAPI
LONG WINAPI trap_handler(EXCEPTION_POINTERS* info);
#else
[[noreturn]] void trap_handler(int, siginfo_t*, void*);
#endif
void register_signal_handler();

class LLVMCodeGen final : public CodeGen {

public:
    explicit LLVMCodeGen(CompilerConfig &);
    ~LLVMCodeGen() override = default;

    [[nodiscard]] string getDescription() override;

    void configure() override;

    void generate(ASTContext *, path) override;
#ifndef _LLVM_LEGACY
    int jit(ASTContext *, path) override;
#endif

private:
    CompilerConfig &config_;
    Logger &logger_;
    OutputFileType type_;
    llvm::TargetMachine *tm_;
    llvm::ExitOnError exitOnErr_;

    void emit(Module *, path, OutputFileType) const;
    static std::string getLibName(const string &, bool, const Triple &);
    static std::string getObjName(const string &, const Triple &);

    void loadObjects(const ASTContext *, LLJIT *) const;

#ifndef _LLVM_LEGACY
    [[nodiscard]] unique_ptr<LLJIT> createLlJit() const;
#endif


};


#endif //OBERON_LANG_LLVMCODEGEN_H
