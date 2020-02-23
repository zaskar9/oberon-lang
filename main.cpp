/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <boost/filesystem.hpp>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/Verifier.h>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "parser/ast/NodePrettyPrinter.h"
#include "codegen/nasm/NASMCodeGen.h"
#include "codegen/nasm/Formatter.h"
#include "codegen/llvm/LLVMCodeGen.h"

using namespace boost::filesystem;

void nasmBackend(std::string filename, Logger* logger, ModuleNode &ast_root) {
    std::string name = change_extension(filename, "asm").string();
    auto assembly = std::make_unique<Assembly>();
    auto codegen = std::make_unique<NASMCodeGen>(assembly.get());
    codegen->visit(ast_root);
    auto formatter = std::make_unique<Formatter>(assembly.get());
    ofstream file(name);
    formatter->format(file);
    file.close();
}

void llvmBackend(std::string filename, Logger* logger, ModuleNode &ast_root) {
    std::string name = change_extension(filename, "ll").string();
    auto codegen = std::make_unique<LLVMCodeGen>(logger);
    codegen->visit(ast_root);
    auto module = codegen->getModule();
    module->setSourceFileName(filename);
    if (!llvm::verifyModule(*module, &errs())) {
        std::error_code ec;
        llvm::raw_fd_ostream file(name, ec);
        if (ec) {
            std::cerr << ec.message() << std::endl;
        } else {
            module->print(file, nullptr);
            file.flush();
        }
        file.close();
    } else {
        logger->error(filename, "code generation failed.");
    }
}

int main(const int argc, const char *argv[]) {
   if (argc != 2) {
      std::cout << "Usage: oberon0c <filename>" << std::endl;
      return 1;
   }
   std::string filename = argv[1];
   auto logger = std::make_unique<Logger>(LogLevel::INFO, &std::cout);
   logger->setLevel(LogLevel::INFO);
   auto scanner = std::make_unique<Scanner>(filename, logger.get());
   auto parser = std::make_unique<Parser>(scanner.get(), logger.get());
   auto ast_root = parser->parse();
   if (logger->getErrorCount() == 0) {
      // auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
      // printer->visit(*ast_root.get());
      // nasmBackend(filename, logger.get(), *ast_root.get());
      llvmBackend(filename, logger.get(), *ast_root.get());
   }
   logger->info(filename, "compilation complete: " +
                    std::to_string(logger->getErrorCount())   + " error(s), " +
                    std::to_string(logger->getWarningCount()) + " warning(s), " +
                    std::to_string(logger->getInfoCount())    + " message(s).");

   // std::cout << "CodeGen.Mod:47:15:{47:8-47:14}{47:17-47:24}: error: invalid operands to binary expression ('int *' and '_Complex float')" << std::endl;
   exit(logger->getErrorCount() != 0);
}