/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#include "LLVMCompiler.h"
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <config.h>
#include "../parser/Parser.h"
#include "../parser/ast/NodePrettyPrinter.h"

LLVMCompiler::LLVMCompiler(Logger *logger) : logger_(logger), ctx_(), pb_(), lvl_(llvm::PassBuilder::O0),
        type_(OutputFileType::ObjectFile) {
    // Initialize LLVM
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    // Use default target triple of host
    std::string triple = llvm::sys::getDefaultTargetTriple();
    // Set up target
    std::string error;
    auto tm = llvm::TargetRegistry::lookupTarget(triple, error);
    if (!tm) {
        logger_->error(PROJECT_NAME, error);
    } else {
        // Set up target machine to match host
        std::string cpu = "generic";
        std::string features = "";
        TargetOptions opt;
        auto model = Optional<Reloc::Model>();
        tm_ = tm->createTargetMachine(triple, cpu, features, opt, model);
    }
}

TargetMachine * LLVMCompiler::getTargetMachine() {
    return tm_;
}

void LLVMCompiler::setOptimizationLevel(llvm::PassBuilder::OptimizationLevel level) {
    lvl_ = level;
}

void LLVMCompiler::setCodeGenFileType(OutputFileType type) {
    type_ = type;
}

void LLVMCompiler::compile(boost::filesystem::path path) {
    // Scan and parse the input file
    auto scanner = std::make_unique<Scanner>(path.string(), logger_);
    auto symbols = std::make_unique<SymbolTable>();
    auto parser = std::make_unique<Parser>(scanner.get(), symbols.get(), logger_);
    auto ast = parser->parse();
    if (ast) {
        // auto printer = std::make_unique<NodePrettyPrinter>(std::cout);
        // printer->visit(*ast.get());
        // Set up the LLVM module
        auto name = path.filename().string();
        auto module = std::make_unique<Module>(name, ctx_);
        module->setDataLayout(tm_->createDataLayout());
        module->setTargetTriple(tm_->getTargetTriple().getTriple());
        // Generate LLVM intermediate representation
        auto builder = std::make_unique<LLVMIRBuilder>(logger_, ctx_, module.get());
        builder->visit(*ast.get());
        module->setSourceFileName(name);
        if (lvl_ != llvm::PassBuilder::O0) {
            // Create basic analyses
            llvm::LoopAnalysisManager lam;
            llvm::FunctionAnalysisManager fam;
            llvm::CGSCCAnalysisManager cgam;
            llvm::ModuleAnalysisManager mam;
            // Register all the basic analyses with the managers
            pb_.registerModuleAnalyses(mam);
            pb_.registerCGSCCAnalyses(cgam);
            pb_.registerFunctionAnalyses(fam);
            pb_.registerLoopAnalyses(lam);
            pb_.crossRegisterProxies(lam, fam, cgam, mam);

            auto mpm = pb_.buildPerModuleDefaultPipeline(lvl_);
            mpm.run(*module.get(), mam);
        }
        if (module) {
            emit(module.get(), path);
        } else {
            logger_->error(path.string(), "code generation failed.");
        }
    }
}

bool LLVMCompiler::emit(Module *module, boost::filesystem::path path) {
    std::string ext;
    switch (type_) {
        case OutputFileType::AssemblyFile:
            ext = "s";
            break;
        case OutputFileType::BitCodeFile:
            ext = "bc";
            break;
        case OutputFileType::LLVMIRFile:
            ext = "ll";
            break;
        default:
#if defined(_WIN32) || defined(_WIN64)
            ext = "obj";
#else
            ext = "o";
#endif
            break;
    }
    std::string name = change_extension(path, ext).string();
    std::error_code ec;
    llvm::raw_fd_ostream output(name, ec, llvm::sys::fs::OF_None);
    if (ec) {
        logger_->error(path.string(), ec.message());
        return true;
    }
    if (type_ == OutputFileType::LLVMIRFile) {
        module->print(output, nullptr);
        output.flush();
        return false;
    }
    if (type_ == OutputFileType::BitCodeFile) {
        llvm::WriteBitcodeToFile(*module, output);
        output.flush();
        return false;
    }
    TargetMachine::CodeGenFileType type;
    switch (type_) {
        case OutputFileType::AssemblyFile:
            type = TargetMachine::CGFT_AssemblyFile;
            break;
        default:
            type = TargetMachine::CGFT_ObjectFile;
            break;
    }
    legacy::PassManager pass;
    if (tm_->addPassesToEmitFile(pass, output, nullptr, type)) {
        logger_->error(path.string(), "target machine cannot emit a file of this type.");
        return true;
    }
    pass.run(*module);
    output.flush();
    return false;
}