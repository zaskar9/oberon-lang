//
// Created by Michael Grossniklaus on 9/16/22.
//

#ifndef OBERON_LANG_COMPILERFLAGS_H
#define OBERON_LANG_COMPILERFLAGS_H


#include <optional>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class CompilerFlags {

private:
    std::vector<fs::path> includes_;

public:
    explicit CompilerFlags() : includes_() {};
    ~CompilerFlags() = default;

    void addIncludeDirectory(fs::path directory);
    std::optional<fs::path> findInclude(fs::path name);

};


#endif //OBERON_LANG_COMPILERFLAGS_H
