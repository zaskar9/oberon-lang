//
// Created by Michael Grossniklaus on 9/16/22.
//

#include "CompilerFlags.h"

void CompilerFlags::addInclude(std::string include) {
    includes_.push_back(include);
}