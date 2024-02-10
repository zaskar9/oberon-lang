//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "Compiler.h"
#include "config.h"
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "analyzer/LambdaLifter.h"
#include "data/ast/NodePrettyPrinter.h"

unique_ptr<ASTContext> Compiler::run(const boost::filesystem::path &file) {
    // Scan, parse, and analyze the input file
    logger_->debug("Parsing and analyzing...");
    auto errors = logger_->getErrorCount();
    auto scanner = std::make_unique<Scanner>(logger_, file);
    auto ast = std::make_unique<ASTContext>();
    auto path = file.parent_path();
    auto importer = std::make_unique<SymbolImporter>(flags_, ast.get(), path, logger_);
    auto exporter = std::make_unique<SymbolExporter>(path, logger_);
    auto sema = std::make_unique<Sema>(ast.get(), system_->getSymbolTable(), importer.get(), exporter.get(), logger_);
    auto parser = std::make_unique<Parser>(flags_, scanner.get(), sema.get(), logger_);
    parser->parse(ast.get());
    if (logger_->getErrorCount() != errors) {
        // Parsing and analyzing failed
        return nullptr;
    }
    auto module = ast->getTranslationUnit();
    // Check if file name matches module name
    if (file.filename().replace_extension("").string() != module->getIdentifier()->name()) {
        std::string name = module->getIdentifier()->name();
        logger_->warning(module->pos(), "module " + name + " should be declared in a file named " + name + ".Mod.");
    }
    // Run AST transformations
    logger_->debug("Transforming...");
    auto analyzer = std::make_unique<Analyzer>(logger_);
    analyzer->add(std::make_unique<LambdaLifter>(ast.get()));
    analyzer->run(module);
#ifdef _DEBUG
    auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
    printer->print(module);
#endif
    return ast;
}

void Compiler::compile(const boost::filesystem::path &file) {
    auto path = absolute(file);
    auto ast = run(file);
    if (ast) {
        codegen_->generate(ast.get(), path.string());
    }
}

#ifndef _LLVM_LEGACY
int Compiler::jit(const boost::filesystem::path &file) {
    auto path = absolute(file);
    auto ast = run(path);
    if (ast) {
        return codegen_->jit(ast.get(), path.string());
    }
    return EXIT_FAILURE;
}
#endif