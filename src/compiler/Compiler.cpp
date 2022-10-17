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

void Compiler::compile(boost::filesystem::path file) {
    // Scan and parse the input file
    logger_->debug(PROJECT_NAME, "parsing...");
    auto fp = absolute(file);
    auto errors = logger_->getErrorCount();
    auto scanner = std::make_unique<Scanner>(fp.string(), logger_);
    auto symbols = std::make_unique<SymbolTable>();
    auto parser = std::make_unique<Parser>(scanner.get(), logger_);

//    for (int i = 0; i < 5; i++) {
//        std::cout << i << ": " << *scanner->peek() << std::endl;
//    }
//    for (int i = 0; i < 5; i++) {
//        std::cout << i << ": " << *scanner->peek(true) << std::endl;
//    }
//    for (int i = 0; i < 5; i++) {
//        std::cout << i << ": " << *scanner->peek() << std::endl;
//    }
//    for (int i = 0; i < 5; i++) {
//        std::cout << i << ": " << *scanner->next() << std::endl;
//    }

    auto ast = parser->parse();
    if (ast /* && ast->getNodeType() == NodeType::module */) {
        // Run the analyzer
        logger_->debug(PROJECT_NAME, "analyzing...");
        auto analyzer = std::make_unique<Analyzer>(logger_);
        auto path = fp.parent_path();
        auto importer = std::make_unique<SymbolImporter>(logger_, flags_, path);
        auto exporter = std::make_unique<SymbolExporter>(logger_, path);
        analyzer->add(std::make_unique<SemanticAnalysis>(symbols.get(), importer.get(), exporter.get()));
        analyzer->add(std::make_unique<LambdaLifter>());
        analyzer->run(ast.get());
        if (logger_->getErrorCount() == errors) {
#ifdef _DEBUG
            auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
            printer->print(ast.get());
#endif
            codegen_->generate(ast.get(), fp.string());
        }

    }
}