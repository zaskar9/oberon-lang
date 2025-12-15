//
// Created by Michael Grossniklaus on 4/3/25.
//

#ifndef OBERON_LANG_LLDWRAPPER_H
#define OBERON_LANG_LLDWRAPPER_H

#include <string>
#include <vector>
#include <llvm/Support/raw_ostream.h>

#include "Logger.h"
#include "compiler/CompilerConfig.h"

using std::string;

namespace lld::elf {
    bool link(llvm::ArrayRef<const char *>, llvm::raw_ostream &, llvm::raw_ostream &, bool, bool);
}

class LLDWrapper {

public:
    explicit LLDWrapper(CompilerConfig &config): config_(config), logger_(config.logger()) {}
    ~LLDWrapper() = default;

    [[nodiscard]] int link() const;
    [[noreturn]] static void destroy(int);

private:
    CompilerConfig &config_;
    Logger &logger_;

    void parseTriple(const string&, vector<string>&) const;

};


#endif //OBERON_LANG_LLDWRAPPER_H
