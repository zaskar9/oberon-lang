/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"

int main(const int argc, const char * argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    auto scanner = new Scanner(argv[1]);
    auto parser = new Parser(scanner);
    const ASTNode* node = parser->parse();
    delete parser;
    delete scanner;
    return 0;
}