//
// Created by Michael Grossniklaus on 4/3/25.
//

#ifndef OBERON_LANG_LLDWRAPPER_H
#define OBERON_LANG_LLDWRAPPER_H

#include <llvm/TargetParser/Triple.h>

using llvm::Triple;

class LLDWrapper {

private:
    Triple triple_;

public:
    explicit LLDWrapper(const Triple &triple): triple_(triple) {}
    ~LLDWrapper() = default;
};


#endif //OBERON_LANG_LLDWRAPPER_H
