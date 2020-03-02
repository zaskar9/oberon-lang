/*
 * Literal tokens returned by scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/1/20.
 */

#include "LiteralToken.h"

void BooleanLiteralToken::print(std::ostream &stream) const {
    stream << this->getType() << ": " << (value() ? "TRUE" : "FALSE");
}
