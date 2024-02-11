//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_CODEGEN_H
#define OBERON_LANG_CODEGEN_H


#include <boost/filesystem.hpp>

#include "global.h"
#include "compiler/CompilerConfig.h"
#include "data/ast/ASTContext.h"

class CodeGen {

public:
    virtual ~CodeGen() noexcept;

    virtual std::string getDescription() = 0;

    virtual void configure() = 0;

    virtual void generate(ASTContext *ast, boost::filesystem::path path) = 0;
#ifndef _LLVM_LEGACY
    virtual int jit(ASTContext *ast, boost::filesystem::path path) = 0;
#endif

};


#endif //OBERON_LANG_CODEGEN_H
