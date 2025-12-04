//
// Created by Michael Grossniklaus on 12/4/25.
//

#ifndef OBERON_LANG_CODEGENFACTORY_H
#define OBERON_LANG_CODEGENFACTORY_H


#include <memory>

#include "CodeGen.h"

using std::unique_ptr;

enum class CompilerBackend { LLVM };

class CodeGenFactory {

public:
    static unique_ptr<CodeGen> GetCodeGen(CompilerBackend, CompilerConfig&);

};


#endif //OBERON_LANG_CODEGENFACTORY_H