/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#ifndef OBERON_LANG_LLVMCODEGEN_H
#define OBERON_LANG_LLVMCODEGEN_H


#include <boost/filesystem.hpp>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Target/TargetMachine.h>
#include "LLVMIRBuilder.h"
#include "../analyzer/Analyzer.h"
#include "../logging/Logger.h"


class LLVMCodeGen {

private:
    Logger *logger_;
    LLVMContext ctx_;
    llvm::PassBuilder pb_;
    llvm::PassBuilder::OptimizationLevel lvl_;
    llvm::TargetMachine *tm_;

    void emit(Module *module, boost::filesystem::path path, OutputFileType type);

public:
    explicit LLVMCodeGen(Logger *logger);
    ~LLVMCodeGen() = default;

    TargetMachine* getTargetMachine();
    void setOptimizationLevel(OptimizationLevel level);

    void generate(Node *ast, boost::filesystem::path path, OutputFileType type);

};


#endif //OBERON_LANG_LLVMCODEGEN_H
