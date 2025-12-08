//
// Created by Michael Grossniklaus on 4/3/25.
//

#ifndef OBERON_LANG_LLDWRAPPER_H
#define OBERON_LANG_LLDWRAPPER_H

#include <string>
#include <vector>

#include "Logger.h"
#include "compiler/CompilerConfig.h"

using std::string;

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
