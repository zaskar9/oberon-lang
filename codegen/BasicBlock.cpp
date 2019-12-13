/*
 * Implementation of the basic block class used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#include "BasicBlock.h"

BasicBlock::BasicBlock(std::string label, std::string comment) :
      label_(std::make_unique<Label>(std::move(label))), comment_(std::move(comment)), instructions_() { };
