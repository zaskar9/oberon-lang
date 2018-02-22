/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    auto logger = new Logger();
    logger->setLevel(LogLevel::ERROR);
    auto scanner = new Scanner(filename, logger);
    auto symbols = new Table(logger);
    auto parser = new Parser(scanner, symbols, logger);
    const ASTNode *node = parser->parse();
    delete parser;
    delete scanner;
    logger->info("", "Compilation complete.");
    exit(0);
}