/*
 * Implementation of the instruction class used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/22/19.
 */

#include "Instruction.h"

Label* Instruction::setLabel(std::string &label) {
    label_ = std::make_unique<Label>(label);
}
