//
// Created by Michael Grossniklaus on 9/16/22.
//

#ifndef OBERON_LANG_COMPILERFLAGS_H
#define OBERON_LANG_COMPILERFLAGS_H


#include <string>
#include <vector>

class CompilerFlags {

private:
    std::vector<std::string> includes_;

public:
    explicit CompilerFlags() : includes_() {};
    ~CompilerFlags() = default;

    void addInclude(std::string include);


};


#endif //OBERON_LANG_COMPILERFLAGS_H
