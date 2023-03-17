/*
 * Global definitions.
 *
 * Created by Michael Grossniklaus on 4/7/20.
 */

#ifndef OBERON_LLVM_GLOBAL_H
#define OBERON_LLVM_GLOBAL_H


#include <string>
#include <sstream>

struct FilePos {
    std::string fileName;
    int lineNo, charNo;
};

static const FilePos EMPTY_POS = { "", 0, 0 };

template <typename T>
static std::string to_string(T obj) {
    std::stringstream stream;
    stream << obj;
    return stream.str();
}

enum class OutputFileType {
    AssemblyFile, BitCodeFile, LLVMIRFile, ObjectFile
};

enum class OptimizationLevel {
    O0, O1, O2, O3
};

enum class RelocationModel {
    DEFAULT, STATIC, PIC
};


#endif //OBERON_LLVM_GLOBAL_H
