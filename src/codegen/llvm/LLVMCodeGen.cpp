/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#include "LLVMCodeGen.h"
#include "LLVMIRBuilder.h"
#include <sstream>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>
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

LLVMCodeGen::LLVMCodeGen(CompilerConfig &config) :
        config_(config), logger_(config_.logger()), type_(OutputFileType::ObjectFile), ctx_(), pb_(),
        lvl_(llvm::OptimizationLevel::O0) {
    // Initialize LLVM
    // TODO some can be skipped when running JIT
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    tm_ = nullptr;
    jit_ = nullptr;
    exitOnErr_.setBanner(std::string(PROJECT_NAME) + ": [error] ");
}

std::string LLVMCodeGen::getDescription() {
    return sys::getDefaultTargetTriple();
}

void LLVMCodeGen::configure() {
    // Set output file type
    type_ = config_.getFileType();
    // Set optimization level
    switch (config_.getOptimizationLevel()) {
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
    std::string triple = config_.getTargetTriple();
    if (triple.empty()) {
        // Use default target triple of host as fallback
        triple = sys::getDefaultTargetTriple();
    }
    logger_.debug("Using target triple: " + triple + ".");
    // Set up target
    std::string error;
    auto target = TargetRegistry::lookupTarget(triple, error);
    if (!target) {
        logger_.error(PROJECT_NAME, error);
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
        switch (config_.getRelocationModel()) {
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
    // TODO Setup for JIT
    if (config_.isJit()) {
        jit_ = exitOnErr_(orc::LLJITBuilder().create());
        // TODO Remove this when this is moved into compiler_rt for JIT
        // If this is a Mingw or Cygwin executor then we need to alias __main to orc_rt_int_void_return_0.
        if (jit_->getTargetTriple().isOSCygMing()) {
            auto dylibsym = jit_->getProcessSymbolsJITDylib()->define(
                    orc::absoluteSymbols({{jit_->mangleAndIntern("__main"),
                                           {orc::ExecutorAddr::fromPtr(mingw_noop_main),
                                            JITSymbolFlags::Exported}}})
            );
        }
        // Load libraries
        for (const auto& name : config_.getLibraries()) {
            logger_.debug("Searching for library: '" + name + "'.");
            auto lib = config_.findLibrary(getLibName(name, true, jit_->getTargetTriple()));
            if (lib) {
                const std::string value(lib.value().string());
                logger_.debug("Loading dynamic library: '" + value + "'.");
                sys::DynamicLibrary::LoadLibraryPermanently(value.c_str());
            } else {
                lib = config_.findLibrary(getLibName(name, false, jit_->getTargetTriple()));
                if (lib) {
                    const std::string value(lib.value().string());
                    logger_.debug("Loading static library: '" + value + "'.");
                    auto &dylib = exitOnErr_(jit_->createJITDylib("name"));
                    exitOnErr_(jit_->linkStaticLibraryInto(dylib, value.c_str()));
                    jit_->getMainJITDylib().addToLinkOrder(dylib);
                } else {
                    logger_.error(PROJECT_NAME, "library not found: '" + name + "'.");
                }
            }
        }
    }
}

std::string LLVMCodeGen::getLibName(const std::string &name, bool dylib, const llvm::Triple &triple) {
    std::stringstream ss;
    if (triple.isOSCygMing()) {
        ss << "lib" << name;
        ss << (dylib ? ".dll" : ".a");
    } else if (triple.isOSWindows()) {
        ss << name << (dylib ? ".dll" : ".lib");
    } else {
        ss << "lib" << name;
        if (dylib) {
            ss << (triple.isMacOSX() ? ".dylib" : ".so");
        } else {
            ss << ".a";
        }
    }
    return ss.str();
}

void LLVMCodeGen::generate(ASTContext *ast, boost::filesystem::path path) {
    // Set up the LLVM module
    logger_.debug("Generating LLVM code...");
    auto name = path.filename().string();
    auto module = std::make_unique<Module>(path.filename().string(), ctx_);
    module->setSourceFileName(path.string());
    module->setDataLayout(tm_->createDataLayout());
    module->setTargetTriple(tm_->getTargetTriple().getTriple());
    // Generate LLVM intermediate representation
    auto builder = std::make_unique<LLVMIRBuilder>(config_, ctx_, module.get());
    builder->build(ast);
    if (lvl_ != llvm::OptimizationLevel::O0) {
        logger_.debug("Optimizing...");
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
    if (module && logger_.getErrorCount() == 0) {
        logger_.debug("Emitting code...");
        emit(module.get(), path, type_);
    } else {
        logger_.error(path.filename().string(), "code generation failed.");
    }
}

#ifndef _LLVM_LEGACY
int LLVMCodeGen::jit(ASTContext *ast, boost::filesystem::path path) {
    // Set up the LLVM module
    logger_.debug("Generating LLVM code...");
    // TODO second context created as LLVMIRBuilder needs std::make_unique
    auto context = std::make_unique<llvm::LLVMContext>();
    auto name = path.filename().string();
    auto module = std::make_unique<Module>(path.filename().string(), *context.get());
    module->setSourceFileName(path.string());
    module->setDataLayout(tm_->createDataLayout());
    module->setTargetTriple(tm_->getTargetTriple().getTriple());
    // Generate LLVM intermediate representation
    auto builder = std::make_unique<LLVMIRBuilder>(config_, *context.get(), module.get());
    builder->build(ast);
    // TODO run optimizer?
    if (module && logger_.getErrorCount() == 0) {
        logger_.debug("Running JIT...");
        exitOnErr_(jit_->addIRModule(orc::ThreadSafeModule(std::move(module), std::move(context))));

        // TODO link with other imported modules (*.o and *.obj files)

        std:: string entry = ast->getTranslationUnit()->getIdentifier()->name();
        auto mainAddr = exitOnErr_(jit_->lookup(entry));
        auto mainFn = mainAddr.toPtr<int(void)>();
        int result = mainFn();
        logger_.debug("Process finished with exit code " + to_string(result));
        return result;
    } else {
        logger_.error(path.filename().string(), "code generation failed.");
    }
    return EXIT_FAILURE;
}
#endif

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
#if (defined(_WIN32) || defined(_WIN64)) && !defined(__MINGW32__)
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
        logger_.error(path.string(), ec.message());
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
#ifdef _LLVM_18
            ft = CodeGenFileType::AssemblyFile;
#else
            ft = CodeGenFileType::CGFT_AssemblyFile;
#endif
            break;
        default:
#ifdef _LLVM_18
            ft = CodeGenFileType::ObjectFile;
#else
            ft = CodeGenFileType::CGFT_ObjectFile;
#endif
            break;
    }
    legacy::PassManager pass;
    if (tm_->addPassesToEmitFile(pass, output, nullptr, ft)) {
        logger_.error(path.string(), "target machine cannot emit a file of this type.");
        return;
    }
    pass.run(*module);
    output.flush();
}