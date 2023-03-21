//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_CODEGEN_H
#define OBERON_LANG_CODEGEN_H


#include <boost/filesystem.hpp>
#include "global.h"
#include "data/ast/Node.h"
#include "compiler/CompilerFlags.h"

class CodeGen {

public:
    virtual ~CodeGen() noexcept;

    virtual std::string getDescription() = 0;

    virtual void configure(CompilerFlags *flags) = 0;

    virtual void generate(Node *ast, boost::filesystem::path path) = 0;

};


#endif //OBERON_LANG_CODEGEN_H
