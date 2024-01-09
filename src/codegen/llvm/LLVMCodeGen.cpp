/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#include "LLVMCodeGen.h"
#include "LLVMIRBuilder.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <config.h>

using namespace llvm;

int mingw_noop_main(void) {
  // Cygwin and MinGW insert calls from the main function to the runtime
  // function __main. The __main function is responsible for setting up main's
  // environment (e.g. running static constructors), however this is not needed
  // when running under lli: the executor process will have run non-JIT ctors,
  // and ORC will take care of running JIT'd ctors. To avoid a missing symbol
  // error we just implement __main as a no-op.
  //
  // FIXME: Move this to ORC-RT (and the ORC-RT substitution library once it
  //        exists). That will allow it to work out-of-process, and for all
  //        ORC tools (the problem isn't lli specific).
  return 0;
}

LLVMCodeGen::LLVMCodeGen(Logger *logger)
        : logger_(logger), type_(OutputFileType::ObjectFile), ctx_(), pb_(), lvl_(llvm::OptimizationLevel::O0) {
    // Initialize LLVM
    // TODO : Some can be skipped when running JIT
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    tm_ = nullptr;
}

std::string LLVMCodeGen::getDescription() {
    return sys::getDefaultTargetTriple();
}

void LLVMCodeGen::configure(CompilerFlags *flags) {
    // TODO : Setup for JIT
    // Set output file type
    type_ = flags->getFileType();
    // Set optimization level
    switch (flags->getOptimizationLevel()) {
        case ::OptimizationLevel::O1:
            lvl_ = llvm::OptimizationLevel::O1;
            break;
        case ::OptimizationLevel::O2:
            lvl_ = llvm::OptimizationLevel::O2;
            break;
        case ::OptimizationLevel::O3:
            lvl_ = llvm::OptimizationLevel::O3;
            break;
        default:
            lvl_ = llvm::OptimizationLevel::O0;
    }
    std::string triple = flags->getTragetTriple();
    if (triple.empty()) {
        // Use default target triple of host as fallback
        triple = sys::getDefaultTargetTriple();
    }
    logger_->debug(PROJECT_NAME, "using target triple: " + triple + ".");
    // Set up target
    std::string error;
    auto target = TargetRegistry::lookupTarget(triple, error);
    if (!target) {
        logger_->error(PROJECT_NAME, error);
    } else {
        // Set up target machine to match host
        std::string cpu = "generic";
        std::string features;
        TargetOptions opt;
#ifdef _LLVM_LEGACY
        auto model = llvm::Optional<Reloc::Model>();
#else
        auto model = std::optional<Reloc::Model>();
#endif
        switch (flags->getRelocationModel()) {
            case RelocationModel::STATIC:
                model = Reloc::Model::Static;
                break;
            case RelocationModel::PIC:
                model = Reloc::Model::PIC_;
                break;
            case RelocationModel::DEFAULT:
            default:
                break;
        }
        tm_ = target->createTargetMachine(triple, cpu, features, opt, model);
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
    if (lvl_ != llvm::OptimizationLevel::O0) {
        logger_->debug(PROJECT_NAME, "optimizing...");
        // Create basic analyses
        LoopAnalysisManager lam;
        FunctionAnalysisManager fam;
        CGSCCAnalysisManager cgam;
        ModuleAnalysisManager mam;
        // Register all the basic analyses with the managers
        pb_.registerModuleAnalyses(mam);
        pb_.registerCGSCCAnalyses(cgam);
        pb_.registerFunctionAnalyses(fam);
        pb_.registerLoopAnalyses(lam);
        pb_.crossRegisterProxies(lam, fam, cgam, mam);
        auto mpm = pb_.buildPerModuleDefaultPipeline(lvl_);
        mpm.run(*module.get(), mam);
    }
    if (module && logger_->getErrorCount() == 0) {
        logger_->debug(PROJECT_NAME, "emitting code...");
        emit(module.get(), path, type_);
    } else {
        logger_->debug(PROJECT_NAME, "code generation failed.");
    }
}

int LLVMCodeGen::jit(Node *ast, boost::filesystem::path path) {
    // Set up the LLVM module
    logger_->debug(PROJECT_NAME, "generating LLVM code...");
    // TODO : Second context created as LLVMIRBuilder needs std::make_unique
    auto Context = std::make_unique<llvm::LLVMContext>();
    auto name = path.filename().string();
    auto module = std::make_unique<Module>(path.filename().string(), *Context.get());
    module->setSourceFileName(path.string());
    module->setDataLayout(tm_->createDataLayout());
    module->setTargetTriple(tm_->getTargetTriple().getTriple());
    // Generate LLVM intermediate representation
    auto builder = std::make_unique<LLVMIRBuilder>(logger_, *Context.get(), module.get());
    builder->build(ast);
    // TODO : Run optimizer?
    if (module && logger_->getErrorCount() == 0) {
        logger_->debug(PROJECT_NAME, "JIT...");

        llvm::orc::LLJITBuilder Builder;
        auto JTMB = llvm::orc::JITTargetMachineBuilder::detectHost();
        auto J = Builder.setJITTargetMachineBuilder(JTMB.get()).create();

        // NOTE : Remove this when this is moved into compiler_rt for JIT
        // If this is a Mingw or Cygwin executor then we need to alias __main to
        // orc_rt_int_void_return_0.
        if (J.get()->getTargetTriple().isOSCygMing()) {
            auto dylibsym = J.get()->getProcessSymbolsJITDylib()->define(
                orc::absoluteSymbols({{J.get()->mangleAndIntern("__main"),
                {orc::ExecutorAddr::fromPtr(mingw_noop_main),
                JITSymbolFlags::Exported}}})
            );
        }

        // TODO : Error handling
        auto H = J.get()->addIRModule(llvm::orc::ThreadSafeModule(std::move(module), std::move(Context)));
        
        // TODO : Link extra object / dll supplied by user
        
        auto MainAddr = J.get()->lookup("main");
        int Result;
        if (!MainAddr) {
            logger_->debug(PROJECT_NAME, "failed to find main entry point.");
        } else {
            using MainFnTy = int32_t();
            auto MainFn = MainAddr->toPtr<MainFnTy *>();
            Result = MainFn();
            logger_->debug(PROJECT_NAME, "return code: " + to_string(Result));
            return Result;
        }
    } else {
        logger_->debug(PROJECT_NAME, "code generation failed.");
    }
    return 1;
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
    std::string name = path.replace_extension(ext).string();
    std::error_code ec;
    raw_fd_ostream output(name, ec, sys::fs::OF_None);
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
        WriteBitcodeToFile(*module, output);
        output.flush();
        return;
    }
    CodeGenFileType ft;
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