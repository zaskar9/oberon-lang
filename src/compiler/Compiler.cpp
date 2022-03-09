//
// Created by Michael Grossniklaus on 3/9/22.
//

#include "Compiler.h"
#include "config.h"
#include "../scanner/Scanner.h"
#include "../parser/Parser.h"
#include "../analyzer/SemanticAnalysis.h"
#include "../analyzer/LambdaLifter.h"

Compiler::Compiler(Logger *logger)  : logger_(logger), type_(OutputFileType::ObjectFile) {
    codegen_ = std::make_unique<LLVMCodeGen>(logger_);
    exporter_ = std::make_unique<SymbolExporter>(logger_);
}

void Compiler::compile(boost::filesystem::path path) {
    // Scan and parse the input file
    logger_->debug(PROJECT_NAME, "parsing...");
    auto errors = logger_->getErrorCount();
    auto scanner = std::make_unique<Scanner>(path.string(), logger_);
    auto symbols = std::make_unique<SymbolTable>();
    auto parser = std::make_unique<Parser>(scanner.get(), logger_);
    auto ast = parser->parse();
    if (ast /* && ast->getNodeType() == NodeType::module */) {
        // Run the analyzer
        logger_->debug(PROJECT_NAME, "analyzing...");
        auto analyzer = std::make_unique<Analyzer>(logger_);
        analyzer->add(std::make_unique<SemanticAnalysis>(symbols.get()));
        analyzer->add(std::make_unique<LambdaLifter>());
        analyzer->run(ast.get());
        if (logger_->getErrorCount() == errors) {
            // auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
            // printer->print(ast.get());
            codegen_->generate(ast.get(), path, type_);
            exporter_->write(ast->getIdentifier()->name(), symbols.get(), path);
        }
    }
}

void Compiler::setCodeGenFileType(OutputFileType type) {
    type_ = type;
}

void Compiler::setOptimizationLevel(OptimizationLevel level) {
    codegen_->setOptimizationLevel(level);
}

std::string Compiler::getBackendDescription() {
    return codegen_->getTargetMachine()->getTargetTriple().str();
}