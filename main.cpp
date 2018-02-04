//
// Created by Michael Grossniklaus on 12/14/17.
//

#include <iostream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"

int main(const int argc, const char * argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    auto sc = new Scanner(argv[1]);
    auto parser = new Parser(sc);
    const ASTNode* node = parser->parse();
    return 0;
}