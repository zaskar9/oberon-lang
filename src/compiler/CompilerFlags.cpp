//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "CompilerFlags.h"

void CompilerFlags::setFileType(OutputFileType type) {
    type_ = type;
}

OutputFileType CompilerFlags::getFileType() {
    return type_;
}

void CompilerFlags::setOptimizationLevel(OptimizationLevel level) {
    level_ = level;
}

OptimizationLevel CompilerFlags::getOptimizationLevel() {
    return level_;
}

void CompilerFlags::setRelocationModel(RelocationModel model) {
    model_ = model;
}

RelocationModel CompilerFlags::getRelocationModel() {
    return model_;
}

void CompilerFlags::addIncludeDirectory(fs::path directory) {
    includes_.push_back(directory);
}

std::optional<fs::path> CompilerFlags::findInclude(fs::path name) {
    for (auto directory : includes_) {
        auto path = directory / name;
        if (fs::exists(path)) {
            return std::optional<fs::path>(path);
        }
    }
    return std::nullopt;
}