/*
 * Global definitions.
 *
 * Created by Michael Grossniklaus on 4/7/20.
 */

#ifndef OBERON_LLVM_GLOBAL_H
#define OBERON_LLVM_GLOBAL_H

#include <sstream>
#include <string>

#if (defined(_WIN32) || defined(_WIN64)) && !(defined(__MINGW64__) || defined(__MINGW32__))
#define _WINAPI
#else
#undef _WINAPI
#endif

struct FilePos {
    std::string fileName;
    int lineNo, charNo;
    std::streampos offset;
};

struct SourcePos {
    int lineNo, charNo;
    std::streampos offset;
};

struct SourceLoc {
    std::string fileName;
    SourcePos start, end;
};

static const FilePos EMPTY_POS = {"", 0, 0, 0 };
static const SourceLoc EMPTY_LOC = {"", {0, 0, 0}, {0, 0, 0}};

template <typename T>
static std::string to_string(T obj) {
    std::stringstream stream;
    stream << obj;
    return stream.str();
}

template <typename T>
static std::string to_string(T *obj) {
    std::stringstream stream;
    stream << *obj;
    return stream.str();
}


#endif //OBERON_LLVM_GLOBAL_H
