/*
 * Global definitions.
 *
 * Created by Michael Grossniklaus on 4/7/20.
 */

#ifndef OBERON_LLVM_GLOBAL_H
#define OBERON_LLVM_GLOBAL_H


#include <string>

struct FilePos {
    std::string fileName;
    int lineNo, charNo;
};

#endif //OBERON_LLVM_GLOBAL_H
