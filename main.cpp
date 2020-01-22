/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "parser/ast/NodePrettyPrinter.h"
#include "codegen/CodeGenerator.h"

using namespace boost::filesystem;

int main(const int argc, const char *argv[]) {
   if (argc != 2) {
      std::cout << "Usage: oberon0c <filename>" << std::endl;
      return 1;
   }
   std::string ifilename = argv[1];
   auto logger = std::make_unique<Logger>(LogLevel::INFO, &std::cout);
   logger->setLevel(LogLevel::INFO);
   auto scanner = std::make_unique<Scanner>(ifilename, logger.get());
   auto parser = std::make_unique<Parser>(scanner.get(), logger.get());
   auto ast_root = parser->parse();
   if (logger->getErrorCount() == 0) {
      // auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
      // printer->visit(*ast_root.get());
      std::string ofilename = change_extension(ifilename, "asm").string();
      logger->debug("", "Starting code generation: writing to \"" + ofilename + "\".");
      auto assembly = std::make_unique<NasmAssembly>();
      auto codegen = std::make_unique<CodeGenerator>(assembly.get());
      codegen->visit(*ast_root.get());
      ofstream ofile(ofilename);
      ofile << *assembly;
      ofile.close();
   }
   logger->info("", "Compilation complete: " +
                    std::to_string(logger->getErrorCount())   + " error(s), " +
                    std::to_string(logger->getWarningCount()) + " warning(s), " +
                    std::to_string(logger->getInfoCount())    + " message(s).");
    exit(0);
}