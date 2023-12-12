//
// Created by Michael Grossniklaus on 9/16/22.
//

#ifndef OBERON_LANG_COMPILERFLAGS_H
#define OBERON_LANG_COMPILERFLAGS_H


#include <boost/filesystem.hpp>
#include <optional>
#include <string>
#include <vector>

namespace fs = boost::filesystem;

enum class OutputFileType {
    AssemblyFile, BitCodeFile, LLVMIRFile, ObjectFile
};

enum class OptimizationLevel {
    O0, O1, O2, O3
};

enum class RelocationModel {
    DEFAULT, STATIC, PIC
};

class CompilerFlags {

private:
    OutputFileType type_;
    OptimizationLevel level_;
    RelocationModel model_;
    std::vector<fs::path> includes_;

public:
    explicit CompilerFlags() : type_(OutputFileType::ObjectFile), level_(OptimizationLevel::O0),
                               model_(RelocationModel::DEFAULT), includes_() {};
    ~CompilerFlags() = default;

    void setFileType(OutputFileType type);
    OutputFileType getFileType();

    void setOptimizationLevel(OptimizationLevel level);
    OptimizationLevel getOptimizationLevel();

    void setRelocationModel(RelocationModel model);
    RelocationModel getRelocationModel();

    void addIncludeDirectory(fs::path directory);
    std::optional<fs::path> findInclude(fs::path name);

};


#endif //OBERON_LANG_COMPILERFLAGS_H
