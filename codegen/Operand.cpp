/*
 * Implementation of the arguments used by the code generator of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 1/25/19.
 */

#include "Operand.h"

const std::string Label::getName() const {
    return name_;
}

const int Immediate::getValue() const {
    return value_;
}
