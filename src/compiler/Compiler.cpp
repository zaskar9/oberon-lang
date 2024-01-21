//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "Compiler.h"
#include "config.h"
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "analyzer/SemanticAnalysis.h"
#include "analyzer/LambdaLifter.h"
#include "data/ast/NodePrettyPrinter.h"

unique_ptr<Node> Compiler::run(const boost::filesystem::path &file) {
    // Scan and parse the input file
    logger_->debug(PROJECT_NAME, "parsing...");
    auto errors = logger_->getErrorCount();
    auto scanner = std::make_unique<Scanner>(file.string(), logger_);
    auto parser = std::make_unique<Parser>(scanner.get(), logger_);
    auto ast = parser->parse();
    if (ast && ast->getNodeType() == NodeType::module) {
        // Run the analyzer
        logger_->debug(PROJECT_NAME, "analyzing...");
        auto analyzer = std::make_unique<Analyzer>(logger_);
        auto path = file.parent_path();
        auto importer = std::make_unique<SymbolImporter>(logger_, flags_, path);
        auto exporter = std::make_unique<SymbolExporter>(logger_, path);
        analyzer->add(std::make_unique<SemanticAnalysis>(system_->getSymbolTable(), importer.get(), exporter.get()));
        analyzer->add(std::make_unique<LambdaLifter>());
        analyzer->run(ast.get());
        if (logger_->getErrorCount() == errors) {
#ifdef _DEBUG
            auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
            printer->print(ast.get());
#endif
            return ast;
        }
    }
    return nullptr;
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