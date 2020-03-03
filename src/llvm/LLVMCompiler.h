/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#ifndef OBERON_LANG_LLVMCOMPILER_H
#define OBERON_LANG_LLVMCOMPILER_H


#include <boost/filesystem.hpp>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include "../util/Logger.h"
#include "LLVMIRBuilder.h"

enum class OutputFileType {
    AssemblyFile, BitCodeFile, LLVMIRFile, ObjectFile
};

class LLVMCompiler {

private:
    Logger *logger_;
    LLVMContext ctx_;
    llvm::PassBuilder pb_;
    llvm::PassBuilder::OptimizationLevel lvl_;
    OutputFileType type_;
    llvm::TargetMachine *tm_;

    bool emit(Module *module, boost::filesystem::path path);

public:
    explicit LLVMCompiler(Logger *logger);
    ~LLVMCompiler() = default;

    TargetMachine* getTargetMachine();
    void setOptimizationLevel(llvm::PassBuilder::OptimizationLevel level);
    void setCodeGenFileType(OutputFileType type);
    void compile(boost::filesystem::path path);

};


#endif //OBERON_LANG_LLVMCOMPILER_H
