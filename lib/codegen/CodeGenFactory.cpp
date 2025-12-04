//
// Created by Michael Grossniklaus on 12/4/25.
//

#include "CodeGenFactory.h"

#include "CodeGen.h"
#include "llvm/LLVMCodeGen.h"

unique_ptr<CodeGen> CodeGenFactory::GetCodeGen(const CompilerBackend type, CompilerConfig &config) {
    switch (type) {
        case CompilerBackend::LLVM:
            return make_unique<LLVMCodeGen>(config);
        default:
            return nullptr;
    }
}
