//
// Created by Michael Grossniklaus on 9/16/22.
//

#ifndef OBERON_LANG_COMPILERCONFIG_H
#define OBERON_LANG_COMPILERCONFIG_H


#include <cstdint>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "Logger.h"

using std::cout;
using std::filesystem::path;
using std::optional;
using std::string;
using std::unordered_set;
using std::vector;

enum class LanguageStandard {
    Oberon90, Oberon0, Oberon2, Oberon07, TurboOberon
};

enum class OutputFileType {
    AssemblyFile, BitCodeFile, LLVMIRFile, ObjectFile
};

enum class OptimizationLevel {
    O0, O1, O2, O3, Os, Oz
};

enum class RelocationModel {
    DEFAULT, STATIC, PIC
};

enum class Flag : uint8_t {
    ENABLE_EXTERN = 1,
    ENABLE_VARARGS = 2,
    ENABLE_MAIN = 3,
    STACK_PROTECT = 4,
    INIT_LOCAL_ZERO = 5,
    INIT_GLOBAL_ZERO = 6,
};

enum class Trap : uint8_t {
    OUT_OF_BOUNDS = 1,
    TYPE_GUARD = 2,
    COPY_OVERFLOW = 3,
    NIL_POINTER = 4,
    PROCEDURE_CALL = 5,
    INT_DIVISION = 6,
    ASSERT = 7,
    INT_OVERFLOW = 8,
    FLT_DIVISION = 9,
    SIGN_CONVERSION = 10,
    TYPE_MISMATCH = 11
};

enum class Warning : unsigned {
    ERROR = 1
};

class CompilerConfig {

public:
    CompilerConfig() : logger_(LogLevel::INFO, cout), std_(LanguageStandard::TurboOberon),
            type_(OutputFileType::ObjectFile), level_(OptimizationLevel::O0), model_(RelocationModel::DEFAULT),
            warn_(0), jit_(false) {
        // Activate default compiler flags
        setSanitizeAll();
        setFlag(Flag::INIT_GLOBAL_ZERO);
        setFlag(Flag::INIT_LOCAL_ZERO);
        setFlag(Flag::STACK_PROTECT);
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

    void setLanguageStandard(LanguageStandard);
    [[nodiscard]] LanguageStandard getLanguageStandard() const;

    void setOutputFile(const string &);
    [[nodiscard]] string getOutputFile() const;

    void setSymbolDirectory(const path &);
    [[nodiscard]] path getSymbolDirectory() const;

    void setInstallDirectory(const path &);
    [[nodiscard]] path getInstallDirectory() const;

    void setWorkingDirectory(const path &);
    [[nodiscard]] path getWorkingDirectory() const;

    void setTargetTriple(const string &);
    [[nodiscard]] string getTargetTriple() const;

    void setFileType(OutputFileType);
    [[nodiscard]] OutputFileType getFileType() const;

    void setOptimizationLevel(OptimizationLevel);
    [[nodiscard]] OptimizationLevel getOptimizationLevel() const;

    void setRelocationModel(RelocationModel);
    [[nodiscard]] RelocationModel getRelocationModel() const;

    void addIncludeDirectory(const path &directory);
    [[nodiscard]] optional<path> findInclude(const path &);

    void addLibraryDirectory(const path &directory);
    [[nodiscard]] optional<path> findLibrary(const path &);

    void addLibrary(const string &);
    [[nodiscard]] const vector<string>& getLibraries() const;

    void setFlag(Flag);
    void toggleFlag(Flag, bool);
    [[nodiscard]] bool hasFlag(Flag) const;

    void toggleSanitize(Trap, bool);
    void setSanitizeAll();
    void setSanitizeNone();
    [[nodiscard]] bool isSanitized(Trap) const;

    void setWarning(Warning);
    [[nodiscard]] bool hasWarning(Warning) const;

    void setJit(bool jit);
    [[nodiscard]] bool isJit() const;

private:
    Logger logger_;
    vector<path> infiles_;
    string outfile_;
    string target_;
    path symboldir_;
    path installdir_;
    path workingdir_;
    LanguageStandard std_;
    OutputFileType type_;
    OptimizationLevel level_;
    RelocationModel model_;
    vector<path> incdirs_;
    vector<path> inc_search_paths_;
    vector<path> libdirs_;
    vector<path> libdircache_;
    vector<string> libs_;
    unordered_set<Flag> flags_;
    unordered_set<Trap> traps_;

    unsigned warn_;
    bool jit_;

    static std::optional<path> find(const path &, const vector<path> &);
    void buildCache(vector<path> &, const vector<path> &, const path &) const;

};


#endif //OBERON_LANG_COMPILERCONFIG_H
