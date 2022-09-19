//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "CompilerFlags.h"

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