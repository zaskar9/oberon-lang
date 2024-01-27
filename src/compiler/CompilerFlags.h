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

enum class Flag : int {
    ENABLE_EXTERN = 1, ENABLE_VARARGS = 2, ENABLE_MAIN = 4
};

class CompilerFlags {

private:
    std::string outfile_;
    std::string target_;
    OutputFileType type_;
    OptimizationLevel level_;
    RelocationModel model_;
    std::vector<fs::path> incpaths_;
    std::vector<fs::path> libpaths_;
    std::vector<std::string> libs_;
    int flags_;
    bool jit_;

    static std::optional<fs::path> find(const fs::path &name, const std::vector<fs::path> &directories);

public:
    CompilerFlags() : outfile_(), target_(), type_(OutputFileType::ObjectFile), level_(OptimizationLevel::O0),
                      model_(RelocationModel::DEFAULT), incpaths_(), libpaths_(), libs_(), flags_(0), jit_(false) {};
    ~CompilerFlags() = default;

    void setOutputFile(std::string file);
    [[nodiscard]] std::string getOutputFile() const;

    void setTargetTriple(std::string target);
    [[nodiscard]] std::string getTragetTriple() const;

    void setFileType(OutputFileType type);
    [[nodiscard]] OutputFileType getFileType() const;

    void setOptimizationLevel(OptimizationLevel level);
    [[nodiscard]] OptimizationLevel getOptimizationLevel() const;

    void setRelocationModel(RelocationModel model);
    [[nodiscard]] RelocationModel getRelocationModel() const;

    void addIncludeDirectory(const fs::path &directory);
    [[nodiscard]] std::optional<fs::path> findInclude(const fs::path &name) const;

    void addLibraryDirectory(const fs::path &directory);
    [[nodiscard]] std::optional<fs::path> findLibrary(const fs::path &name) const;

    void addLibrary(const std::string &name);
    [[nodiscard]] const std::vector<std::string>& getLibraries() const;

    void setFlag(Flag flag);
    [[nodiscard]] bool hasFlag(Flag flag) const;

    void setJit(bool jit);
    [[nodiscard]] bool isJit();

};


#endif //OBERON_LANG_COMPILERFLAGS_H
