//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "CompilerFlags.h"

#include <utility>

void CompilerFlags::setOutputFile(std::string file) {
    outfile_ = file;
}

std::string CompilerFlags::getOutputFile() const {
    return outfile_;
}

void CompilerFlags::setTargetTriple(std::string target) {
    target_ = target;
}

std::string CompilerFlags::getTragetTriple() const {
    return target_;
}

void CompilerFlags::setFileType(OutputFileType type) {
    type_ = type;
}

OutputFileType CompilerFlags::getFileType() const {
    return type_;
}

void CompilerFlags::setOptimizationLevel(OptimizationLevel level) {
    level_ = level;
}

OptimizationLevel CompilerFlags::getOptimizationLevel() const {
    return level_;
}

void CompilerFlags::setRelocationModel(RelocationModel model) {
    model_ = model;
}

RelocationModel CompilerFlags::getRelocationModel() const {
    return model_;
}

std::optional<fs::path> CompilerFlags::find(const fs::path &name, const std::vector<fs::path> &directories) {
    for (auto const &directory : directories) {
        auto path = directory / name;
        if (fs::exists(path)) {
            return { path };
        }
    }
    return std::nullopt;

}

void CompilerFlags::addIncludeDirectory(const fs::path &directory) {
    incpaths_.push_back(directory);
}

std::optional<fs::path> CompilerFlags::findInclude(const fs::path &name) const {
    return find(name, incpaths_);
}

void CompilerFlags::addLibraryDirectory(const fs::path &directory) {
    libpaths_.push_back(directory);
}

std::optional<fs::path> CompilerFlags::findLibrary(const fs::path &name) const {
    return find(name, libpaths_);
}

void CompilerFlags::addLibrary(const std::string &name) {
    libs_.push_back(name);
}

const std::vector<std::string> &CompilerFlags::getLibraries() const {
    return libs_;
}

void CompilerFlags::setFlag(Flag flag) {
    flags_ |= static_cast<int>(flag);
}

bool CompilerFlags::hasFlag(Flag flag) const {
    return flags_ & static_cast<int>(flag);
}

void CompilerFlags::setJit(bool jit) {
    jit_ = jit;
}

bool CompilerFlags::isJit() {
    return jit_;
}