//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "CompilerConfig.h"


Logger &CompilerConfig::logger() {
    return logger_;
}

void CompilerConfig::setLanguageStandard(const LanguageStandard std) {
    std_ = std;
}

LanguageStandard CompilerConfig::getLanguageStandard() const {
    return std_;
}


void CompilerConfig::setOutputFile(const string &file) {
    outfile_ = file;
}

string CompilerConfig::getOutputFile() const {
    return outfile_;
}

void CompilerConfig::setSymDir(const string &dir) {
    symdir_ = dir;
}

string CompilerConfig::getSymDir() const {
    return symdir_;
}

void CompilerConfig::setTargetTriple(const string &target) {
    target_ = target;
}

string CompilerConfig::getTargetTriple() const {
    return target_;
}

void CompilerConfig::setFileType(const OutputFileType type) {
    type_ = type;
}

OutputFileType CompilerConfig::getFileType() const {
    return type_;
}

void CompilerConfig::setOptimizationLevel(const OptimizationLevel level) {
    level_ = level;
}

OptimizationLevel CompilerConfig::getOptimizationLevel() const {
    return level_;
}

void CompilerConfig::setRelocationModel(const RelocationModel model) {
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

void CompilerConfig::setFlag(const Flag flag) {
    toggleFlag(flag, true);
}

void CompilerConfig::toggleFlag(const Flag flag, const bool active) {
    if (active) {
        flags_.insert(flag);
    } else {
        flags_.erase(flag);
    }
}

bool CompilerConfig::hasFlag(const Flag flag) const {
    return flags_.contains(flag);
}

void CompilerConfig::toggleSanitize(const Trap trap, const bool active) {
    if (active) {
        traps_.insert(trap);
    } else {
        traps_.erase(trap);
    }
}

void CompilerConfig::setSanitizeAll() {
    traps_.insert({Trap::OUT_OF_BOUNDS, Trap::TYPE_GUARD, Trap::COPY_OVERFLOW, Trap::NIL_POINTER,
                   Trap::PROCEDURE_CALL, Trap::INT_DIVISION, Trap::ASSERT, Trap::INT_OVERFLOW,
                   Trap::FLT_DIVISION, Trap::SIGN_CONVERSION});
}

void CompilerConfig::setSanitizeNone() {
    traps_.clear();
}

bool CompilerConfig::isSanitized(const Trap trap) const {
    return traps_.contains(trap);
}

void CompilerConfig::setWarning(Warning warn) {
    warn_ |= static_cast<unsigned>(warn);
}

bool CompilerConfig::hasWarning(Warning warn) const {
    return warn_ & static_cast<unsigned>(warn);
}

void CompilerConfig::setJit(const bool jit) {
    jit_ = jit;
}

bool CompilerConfig::isJit() const {
    return jit_;
}