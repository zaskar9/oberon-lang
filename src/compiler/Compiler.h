//
// Created by Michael Grossniklaus on 3/9/22.
//

#ifndef OBERON_LANG_COMPILER_H
#define OBERON_LANG_COMPILER_H


#include <boost/filesystem.hpp>
#include "../logging/Logger.h"
#include "../llvm/LLVMCodeGen.h"
#include "../data/symtab/SymbolExporter.h"

class Compiler {

private:
    Logger *logger_;
    OutputFileType type_;
    std::unique_ptr<LLVMCodeGen> codegen_;
    std::unique_ptr<SymbolExporter> exporter_;

public:
    explicit Compiler(Logger *logger);
    ~Compiler() = default;

    void compile(boost::filesystem::path path);
    void setCodeGenFileType(OutputFileType type);
    void setOptimizationLevel(OptimizationLevel level);
    std::string getBackendDescription();

};


#endif //OBERON_LANG_COMPILER_H
