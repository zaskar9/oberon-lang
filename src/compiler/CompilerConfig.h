//
// Created by Michael Grossniklaus on 9/16/22.
//

#ifndef OBERON_LANG_COMPILERCONFIG_H
#define OBERON_LANG_COMPILERCONFIG_H


#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "logging/Logger.h"

using std::cout;
using std::filesystem::path;
using std::optional;
using std::string;
using std::unordered_set;
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

enum class Flag : unsigned {
    ENABLE_EXTERN = 1,
    ENABLE_VARARGS = 2,
    ENABLE_MAIN = 4,
    NO_STACK_PROTECT = 8
};

enum class Trap : unsigned {
    OUT_OF_BOUNDS = 1,
    TYPE_GUARD = 2,
    COPY_OVERFLOW = 3,
    NIL_POINTER = 4,
    PROCEDURE_CALL = 5,
    INT_DIVISION = 6,
    ASSERT = 7,
    INT_OVERFLOW = 8,
    FLT_DIVISION = 9
};

enum class Warning : unsigned {
    ERROR = 1
};

class CompilerConfig {

private:
    Logger logger_;
    vector<path> infiles_;
    string outfile_;
    string target_;
    string symdir_;
    OutputFileType type_;
    OptimizationLevel level_;
    RelocationModel model_;
    vector<path> incpaths_;
    vector<path> libpaths_;
    vector<string> libs_;
    unsigned flags_;
    unordered_set<Trap> traps_;
    unsigned warn_;
    bool jit_;

    static std::optional<path> find(const path &name, const vector<path> &directories);

public:
    CompilerConfig() : logger_(LogLevel::INFO, cout),
            infiles_(), outfile_(), target_(), symdir_(), type_(OutputFileType::ObjectFile), level_(OptimizationLevel::O0),
            model_(RelocationModel::DEFAULT), incpaths_(), libpaths_(), libs_(), flags_(0), traps_(), warn_(0), jit_(false) {
        setSanitizeAll();
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

    void setSymDir(const string &dir);
    [[nodiscard]] string getSymDir() const;
    
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

    void toggleSanitize(Trap, bool);
    void setSanitizeAll();
    void setSanitizeNone();
    [[nodiscard]] bool isSanitized(Trap) const;

    void setWarning(Warning warn);
    [[nodiscard]] bool hasWarning(Warning warn) const;

    void setJit(bool jit);
    [[nodiscard]] bool isJit();

};


#endif //OBERON_LANG_COMPILERCONFIG_H
