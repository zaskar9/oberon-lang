//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "CompilerConfig.h"


Logger &CompilerConfig::logger() {
    return logger_;
}

void CompilerConfig::setOutputFile(const string &file) {
    outfile_ = file;
}

string CompilerConfig::getOutputFile() const {
    return outfile_;
}

void CompilerConfig::setTargetTriple(const string &target) {
    target_ = target;
}

string CompilerConfig::getTargetTriple() const {
    return target_;
}

void CompilerConfig::setFileType(OutputFileType type) {
    type_ = type;
}

OutputFileType CompilerConfig::getFileType() const {
    return type_;
}

void CompilerConfig::setOptimizationLevel(OptimizationLevel level) {
    level_ = level;
}

OptimizationLevel CompilerConfig::getOptimizationLevel() const {
    return level_;
}

void CompilerConfig::setRelocationModel(RelocationModel model) {
    model_ = model;
}

RelocationModel CompilerConfig::getRelocationModel() const {
    return model_;
}

optional<path> CompilerConfig::find(const path &name, const vector<path> &directories) {
    for (auto const &directory : directories) {
        auto path = directory / name;
        if (std::filesystem::exists(path)) {
            return { path };
        }
    }
    return std::nullopt;

}

void CompilerConfig::addIncludeDirectory(const path &directory) {
    incpaths_.push_back(directory);
}

optional<path> CompilerConfig::findInclude(const path &name) const {
    return find(name, incpaths_);
}

void CompilerConfig::addLibraryDirectory(const path &directory) {
    libpaths_.push_back(directory);
}

optional<path> CompilerConfig::findLibrary(const path &name) const {
    return find(name, libpaths_);
}

void CompilerConfig::addLibrary(const string &name) {
    libs_.push_back(name);
}

const vector<string> &CompilerConfig::getLibraries() const {
    return libs_;
}

void CompilerConfig::setFlag(Flag flag) {
    flags_ |= static_cast<int>(flag);
}

bool CompilerConfig::hasFlag(Flag flag) const {
    return flags_ & static_cast<int>(flag);
}

void CompilerConfig::setJit(bool jit) {
    jit_ = jit;
}

bool CompilerConfig::isJit() {
    return jit_;
}