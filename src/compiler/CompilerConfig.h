//
// Created by Michael Grossniklaus on 9/16/22.
//

#ifndef OBERON_LANG_COMPILERCONFIG_H
#define OBERON_LANG_COMPILERCONFIG_H


#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "logging/Logger.h"

using std::cout;
using std::filesystem::path;
using std::optional;
using std::string;
using std::vector;

enum class OutputFileType {
    AssemblyFile, BitCodeFile, LLVMIRFile, ObjectFile
};

enum class OptimizationLevel {
    O0, O1, O2, O3
};

enum class RelocationModel {
    DEFAULT, STATIC, PIC
};

enum class Flag : int {
    ENABLE_EXTERN = 1, ENABLE_VARARGS = 2, ENABLE_MAIN = 4
};

class CompilerConfig {

private:
    Logger logger_;
    vector<path> infiles_;
    string outfile_;
    string target_;
    OutputFileType type_;
    OptimizationLevel level_;
    RelocationModel model_;
    vector<path> incpaths_;
    vector<path> libpaths_;
    vector<string> libs_;
    int flags_;
    bool jit_;

    static std::optional<path> find(const path &name, const vector<path> &directories);

public:
    CompilerConfig() : logger_(LogLevel::INFO, cout),
            infiles_(), outfile_(), target_(), type_(OutputFileType::ObjectFile), level_(OptimizationLevel::O0),
            model_(RelocationModel::DEFAULT), incpaths_(), libpaths_(), libs_(), flags_(0), jit_(false) {
#ifdef _DEBUG
        logger_.setLevel(LogLevel::DEBUG);
#endif
    };
    CompilerConfig(const CompilerConfig &) = delete;
    CompilerConfig& operator=(CompilerConfig &) = delete;
    ~CompilerConfig() = default;

    Logger &logger();

    void addInputFile(path file);
    [[nodiscard]] vector<path> &inputFiles();

    void setOutputFile(const string &file);
    [[nodiscard]] string getOutputFile() const;

    void setTargetTriple(const string &target);
    [[nodiscard]] string getTargetTriple() const;

    void setFileType(OutputFileType type);
    [[nodiscard]] OutputFileType getFileType() const;

    void setOptimizationLevel(OptimizationLevel level);
    [[nodiscard]] OptimizationLevel getOptimizationLevel() const;

    void setRelocationModel(RelocationModel model);
    [[nodiscard]] RelocationModel getRelocationModel() const;

    void addIncludeDirectory(const path &directory);
    [[nodiscard]] optional<path> findInclude(const path &name) const;

    void addLibraryDirectory(const path &directory);
    [[nodiscard]] optional<path> findLibrary(const path &name) const;

    void addLibrary(const string &name);
    [[nodiscard]] const vector<string>& getLibraries() const;

    void setFlag(Flag flag);
    [[nodiscard]] bool hasFlag(Flag flag) const;

    void setJit(bool jit);
    [[nodiscard]] bool isJit();

};


#endif //OBERON_LANG_COMPILERCONFIG_H
