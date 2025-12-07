//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "Compiler.h"

#include <filesystem>

#include "Scanner.h"
#include "parser/Parser.h"
#include "analyzer/LambdaLifter.h"
#include "data/ast/NodePrettyPrinter.h"

using std::filesystem::path;

unique_ptr<ASTContext> Compiler::run(const path &file) const {
    // Scan, parse, and analyze the input file
    logger_.debug("Parsing and analyzing...");
    const int errors = logger_.getErrorCount();
    Scanner scanner(logger_, file);
    auto ast = std::make_unique<ASTContext>(file);
    Sema sema(config_, ast.get(), system_.get());
    Parser parser(config_, scanner, sema);
    parser.parse(ast.get());
    if (logger_.getErrorCount() != errors) {
        // Parsing and analyzing failed
        return nullptr;
    }
    const auto module = ast->getTranslationUnit();
    // Check if file name matches module name
    if (file.filename().replace_extension("").string() != module->getIdentifier()->name()) {
        const std::string name = module->getIdentifier()->name();
        logger_.warning(module->pos(), "module " + name + " should be declared in a file named " + name + ".Mod.");
    }
    // Run AST transformations
    logger_.debug("Transforming...");
    const auto analyzer = std::make_unique<Analyzer>(config_);
    analyzer->add(std::make_unique<LambdaLifter>(ast.get()));
    analyzer->run(module);
#ifdef _DEBUG
    const auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
    printer->print(module);
#endif
    return ast;
}

void Compiler::compile(const path &file) const {
    const auto path = absolute(file);
    const auto ast = run(file);
    if (ast) {
        codegen_->generate(ast.get(), path.string());
    }
}

#ifndef _LLVM_LEGACY
int Compiler::jit(const path &file) const {
    const auto path = absolute(file);
    const auto ast = run(path);
    if (ast) {
        return codegen_->jit(ast.get(), path.string());
    }
    return EXIT_FAILURE;
}
#endif