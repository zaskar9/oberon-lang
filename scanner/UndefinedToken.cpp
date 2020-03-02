/*
 * Undefined token returned scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/28/18.
 */

#include "UndefinedToken.h"

char UndefinedToken::getValue() {
    return value_;
}

void UndefinedToken::print(std::ostream &stream) const {
    stream << this->type() << ": " << value_;
}