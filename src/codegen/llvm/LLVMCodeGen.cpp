/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#include "LLVMCodeGen.h"
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <config.h>

LLVMCodeGen::LLVMCodeGen(Logger *logger)
        : logger_(logger), type_(OutputFileType::ObjectFile), ctx_(), pb_(), lvl_(llvm::PassBuilder::OptimizationLevel::O0) {
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
        std::string features;
        TargetOptions opt;
        auto model = Optional<Reloc::Model>();
        tm_ = tm->createTargetMachine(triple, cpu, features, opt, model);
    }
}

std::string LLVMCodeGen::getDescription() {
    return tm_->getTargetTriple().str();
}

void LLVMCodeGen::setFileType(OutputFileType type) {
    type_ = type;
}

void LLVMCodeGen::setOptimizationLevel(OptimizationLevel level) {
    switch (level) {
        case OptimizationLevel::O1:
            lvl_ = PassBuilder::OptimizationLevel::O1;
            break;
        case OptimizationLevel::O2:
            lvl_ = PassBuilder::OptimizationLevel::O2;
            break;
        case OptimizationLevel::O3:
            lvl_ = PassBuilder::OptimizationLevel::O3;
            break;
        default:
            lvl_ = PassBuilder::OptimizationLevel::O0;
    }
}

void LLVMCodeGen::generate(Node *ast, boost::filesystem::path path) {
    // Set up the LLVM module
    logger_->debug(PROJECT_NAME, "generating LLVM code...");
    auto name = path.filename().string();
    auto module = std::make_unique<Module>(path.filename().string(), ctx_);
    module->setSourceFileName(path.string());
    module->setDataLayout(tm_->createDataLayout());
    module->setTargetTriple(tm_->getTargetTriple().getTriple());
    // Generate LLVM intermediate representation
    auto builder = std::make_unique<LLVMIRBuilder>(logger_, ctx_, module.get());
    builder->build(ast);
    if (lvl_ != llvm::PassBuilder::OptimizationLevel::O0) {
        logger_->debug(PROJECT_NAME, "optimizing...");
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
        logger_->debug(PROJECT_NAME, "emitting code...");
        emit(module.get(), path, type_);
    } else {
        logger_->error(path.string(), "code generation failed.");
    }
}

void LLVMCodeGen::emit(Module *module, boost::filesystem::path path, OutputFileType type) {
    std::string ext;
    switch (type) {
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
        return;
    }
    if (type == OutputFileType::LLVMIRFile) {
        module->print(output, nullptr);
        output.flush();
        return;
    }
    if (type == OutputFileType::BitCodeFile) {
        llvm::WriteBitcodeToFile(*module, output);
        output.flush();
        return;
    }
    llvm::CodeGenFileType ft;
    switch (type) {
        case OutputFileType::AssemblyFile:
            ft = CGFT_AssemblyFile;
            break;
        default:
            ft = CGFT_ObjectFile;
            break;
    }
    legacy::PassManager pass;
    if (tm_->addPassesToEmitFile(pass, output, nullptr, ft)) {
        logger_->error(path.string(), "target machine cannot emit a file of this type.");
        return;
    }
    pass.run(*module);
    output.flush();
}