/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "parser/ast/NodePrettyPrinter.h"

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    auto logger = std::make_unique<Logger>();
    logger->setLevel(LogLevel::INFO);
    auto scanner = std::make_unique<Scanner>(filename, logger.get());
    auto parser = std::make_unique<Parser>(scanner.get(), logger.get());
    auto ast_root = parser->parse();
    auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
    // printer->visit(*ast_root.get());
    logger->info("", "Compilation complete.");
    exit(0);
}