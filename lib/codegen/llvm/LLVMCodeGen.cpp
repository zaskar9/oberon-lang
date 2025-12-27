/*
 * Implementation of a simple compiler based on the LLVM compiler infrastructure.
 *
 * Created by Michael Grossniklaus on 2/29/20.
 */

#include "LLVMCodeGen.h"
#include <sstream>
#include <unordered_set>

#include <boost/predef.h>

#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>

#include "LLVMIRBuilder.h"

using llvm::PassBuilder;

int mingw_noop_main() {
  // Cygwin and MinGW insert calls from the main function to the runtime
  // function __main. The __main function is responsible for setting up main's
  // environment (e.g. running static constructors), however this is not needed
  // when running under lli: the executor process will have run non-JIT constructors,
  // and ORC will take care of running JIT'd constructors. To avoid a missing symbol
  // error we just implement __main as a no-op.
  //
  // FIXME: Move this to ORC-RT (and the ORC-RT substitution library once it
  //        exists). That will allow it to work out-of-process, and for all
  //        ORC tools (the problem isn't lli specific).
  return 0;
}

[[noreturn]] void ubsantrap_handler(const uint16_t code) {
    std::cerr << std::endl << "Oberon program trapped with code " << code;
    switch (code) {
        case  1: std::cerr << " (array index out of range)"; break;
        case  2: std::cerr << " (type guard failure)"; break;
        case  3: std::cerr << " (array or string copy overflow)"; break;
        case  4: std::cerr << " (access via NIL pointer)"; break;
        case  5: std::cerr << " (illegal procedure call)"; break;
        case  6: std::cerr << " (integer division by zero)"; break;
        case  7: std::cerr << " (assertion violated)"; break;
        case  8: std::cerr << " (integer overflow)"; break;
        case  9: std::cerr << " (floating point division by zero)"; break;
        case 10: std::cerr << " (implicit sign conversion)"; break;
        case 11: std::cerr << " (dynamic type mismatch)"; break;
        default: break;
    }
    std::cerr << std::endl;
    _exit(1);
}

uint16_t decode_trap(void *addr) {
    uint16_t code = 0;
#if BOOST_ARCH_ARM
    // Get the address of the trapped instruction from info->si_addr
    const auto pc = static_cast<uint32_t*>(addr);
    // Read the instruction at the trapped PC
    uint32_t instr = *pc;
    // Check if it is a `BRK` instruction (0xD4200000 mask)
    if ((instr & 0xFFE00000) == 0xD4200000) {
        // Mask out the opcode and extract the immediate (lower 16 bits)
        code = instr >> 5 & 0xFF;
    }
#elif BOOST_ARCH_X86
    std::unordered_set<uint8_t> prefixes({ 0xF0, 0xF2, 0xF3, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65, 0x66, 0x67 });
    auto instr = static_cast<uint8_t*>(addr);
    size_t pos = 0;
    // Skip Prefixes
    while (true) {
        if (prefixes.contains(instr[pos]) || (instr[pos] >= 0x40 && instr[pos] <= 0x4F)) {
            ++pos;
            continue;
        }
        break;
    }
    // Check if it is a `UD1` instruction (0F B9 opcode)
    if (instr[pos] == 0x0F || instr[pos + 1] == 0xB9) {
        pos += 3;  // Move past the opcode and ModR/M byte
        code = static_cast<uint8_t>(instr[pos]);
    }
#else
    #error Unsupported Architecture
#endif
    return code;
}

#ifdef _WINAPI
LONG WINAPI trap_handler(EXCEPTION_POINTERS* info) {
    if (info->ExceptionRecord->ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION ||
        info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT) {
#if BOOST_ARCH_ARM
        auto pc = (void*)info->ContextRecord->Pc;
#elif BOOST_ARCH_X86_64
        auto pc = (void*)info->ContextRecord->Rip;
#elif BOOST_ARCH_X86_32
        auto pc = (void*)info->ContextRecord->Eip;
#else
    #error Unsupported Architecture
#endif
        uint16_t code = decode_trap(pc);
        ubsantrap_handler(code);
        // Handle and continue execution
        // return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}
#else
[[noreturn]] void trap_handler(int, siginfo_t* info, void*) {
    const auto code = decode_trap(info->si_addr);
    ubsantrap_handler(code);
}
#endif

void register_signal_handler() {
#ifdef _WINAPI
    AddVectoredExceptionHandler(1, trap_handler);
#else
    struct sigaction sa{};
    sa.sa_sigaction = trap_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGTRAP, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
#endif
}


LLVMCodeGen::LLVMCodeGen(CompilerConfig &config) :
        config_(config), logger_(config_.logger()), type_(OutputFileType::ObjectFile) {
    // Initialize LLVM
    // TODO some can be skipped when running JIT
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();
    tm_ = nullptr;
    exitOnErr_.setBanner(logger_.getBanner() + ": [error] ");
}

std::string LLVMCodeGen::getDescription() {
    return sys::getDefaultTargetTriple();
}

void LLVMCodeGen::configure() {
    // Set output file type
    type_ = config_.getFileType();
    std::string triple = config_.getTargetTriple();
    if (triple.empty()) {
        // Use default target triple of host as fallback
        triple = sys::getDefaultTargetTriple();
    }
    logger_.debug("Using target triple: " + triple + ".");
    // Set up target
    string error;
    if (const auto target = TargetRegistry::lookupTarget(triple, error); !target) {
        logger_.error(string(), error);
    } else {
        // Set up target machine to match host
        const string cpu = "generic";
        const string features;
        const TargetOptions opt;
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
#if defined(_LLVM_21)
        const Triple t(triple);
        tm_ = target->createTargetMachine(t, cpu, features, opt, model);
#else
        tm_ = target->createTargetMachine(triple, cpu, features, opt, model);
#endif
    }
}

std::string LLVMCodeGen::getLibName(const std::string &name, const bool dylib, const Triple &triple) {
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

std::string LLVMCodeGen::getObjName(const std::string &name, const Triple &triple) {
    std::stringstream ss;
    ss << name;
    if (triple.isOSWindows() && !triple.isOSCygMing()) {
        ss << ".obj";
    } else {
        ss << ".o";
    }
    return ss.str();
}

void LLVMCodeGen::loadObjects(const ASTContext *ast, LLJIT *jit) const {
    const auto module = ast->getTranslationUnit();
    for (const auto& imp: module->imports()) {
        const auto name = imp->getModule()->name();
        logger_.debug("Searching object file for module: '" + name + "'.");
        if (const auto obj = config_.findLibrary(getObjName(name, jit->getTargetTriple()))) {
            // Load the object file from a file
            string file = obj->string();
            logger_.debug("Object file found: '" + file + "'.");
            auto buffer = MemoryBuffer::getFile(file);
            if (auto ec = buffer.getError()) {
                logger_.error(file, ec.message());
            }
            // Add the object file to the JIT
            if (auto error = jit->addObjectFile(std::move(buffer.get()))) {
                logger_.error(file, toString(std::move(error)));
            }
            logger_.debug("Object file loaded: '" + file + "'.");
        }
    }
}

unique_ptr<LLJIT> LLVMCodeGen::createLlJit() const {
    auto jit = exitOnErr_(orc::LLJITBuilder().create());
    // TODO Remove this when this is moved into compiler_rt for JIT
    // If this is a Mingw or Cygwin executor then we need to alias __main to orc_rt_int_void_return_0.
    if (jit->getTargetTriple().isOSCygMing()) {
        auto dylibsym = jit->getProcessSymbolsJITDylib()->define(
                orc::absoluteSymbols({{jit->mangleAndIntern("__main"),
                                       {orc::ExecutorAddr::fromPtr(mingw_noop_main),
                                        JITSymbolFlags::Exported}}})
        );
    }
    // Add symbols from the current REPL (read-execute-print-loop) process
    const auto layout = jit->getDataLayout();
    jit->getMainJITDylib().addGenerator(
            cantFail(orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(layout.getGlobalPrefix())));
    // Load libraries
    for (const auto& name : config_.getLibraries()) {
        logger_.debug("Searching for library: '" + name + "'.");
        if (auto lib = config_.findLibrary(getLibName(name, true, jit->getTargetTriple()))) {
            const std::string value(lib.value().string());
            logger_.debug("Loading dynamic library: '" + value + "'.");
            sys::DynamicLibrary::LoadLibraryPermanently(value.c_str());
        } else {
            lib = config_.findLibrary(getLibName(name, false, jit->getTargetTriple()));
            if (lib) {
                const std::string value(lib.value().string());
                logger_.debug("Loading static library: '" + value + "'.");
                auto &dylib = exitOnErr_(jit->createJITDylib("name"));
                exitOnErr_(jit->linkStaticLibraryInto(dylib, value.c_str()));
                jit->getMainJITDylib().addToLinkOrder(dylib);
            } else {
                logger_.error(string(), "library not found: '" + name + "'.");
            }
        }
    }
    return jit;
}

void LLVMCodeGen::generate(ASTContext *ast, const path path) {
    // Set up the LLVM module
    logger_.debug("Generating LLVM code...");
    auto name = path.filename().string();
    const auto context = std::make_unique<LLVMContext>();
    auto module = std::make_unique<Module>(path.filename().string(), *context);
    module->setSourceFileName(path.string());
    module->setDataLayout(tm_->createDataLayout());
#ifndef _LLVM_21
    module->setTargetTriple(tm_->getTargetTriple().getTriple());
#else
    module->setTargetTriple(tm_->getTargetTriple());
#endif
    // Generate LLVM intermediate representation
    auto builder = std::make_unique<LLVMIRBuilder>(config_, *context, module.get());
    builder->build(ast);
    logger_.debug("Analyzing...");
     // Create basic analyses
     LoopAnalysisManager lam;
     FunctionAnalysisManager fam;
     CGSCCAnalysisManager cgam;
     ModuleAnalysisManager mam;
     // Create a new pass manager builder
     PassBuilder pb;
     // Register all the basic analyses with the managers
     pb.registerModuleAnalyses(mam);
     pb.registerCGSCCAnalyses(cgam);
     pb.registerFunctionAnalyses(fam);
     pb.registerLoopAnalyses(lam);
     pb.crossRegisterProxies(lam, fam, cgam, mam);
     ModulePassManager mpm;
     llvm::OptimizationLevel lvl;
     switch (config_.getOptimizationLevel()) {
         case ::OptimizationLevel::O1:
             lvl = llvm::OptimizationLevel::O1;
             break;
         case ::OptimizationLevel::O2:
             lvl = llvm::OptimizationLevel::O2;
             break;
         case ::OptimizationLevel::O3:
             lvl = llvm::OptimizationLevel::O3;
             break;
         case ::OptimizationLevel::Os:
             lvl = llvm::OptimizationLevel::Os;
             break;
         case ::OptimizationLevel::Oz:
             lvl = llvm::OptimizationLevel::Oz;
             break;
         default:
             lvl = llvm::OptimizationLevel::O0;
             break;
     }
     if (lvl == llvm::OptimizationLevel::O0) {
         mpm = pb.buildO0DefaultPipeline(lvl);
     } else {
         mpm = pb.buildPerModuleDefaultPipeline(lvl);
     }
     logger_.debug("Optimizing...");
     mpm.run(*module, mam);
     if (module && logger_.getErrorCount() == 0) {
         logger_.debug("Emitting code...");
         emit(module.get(), path, type_);
     } else {
         logger_.error(path.filename().string(), "code generation failed.");
     }
}

#ifndef _LLVM_LEGACY
int LLVMCodeGen::jit(ASTContext *ast, const path path) {
    const auto jit = createLlJit();
    // Set up the LLVM module
    logger_.debug("Generating LLVM code...");
    // TODO second context created as LLVMIRBuilder needs std::make_unique
    auto context = std::make_unique<LLVMContext>();
    auto name = path.filename().string();
    auto module = std::make_unique<Module>(path.filename().string(), *context);
    module->setSourceFileName(path.string());
    // module->setDataLayout(tm_->createDataLayout());
#ifndef _LLVM_21
    module->setTargetTriple(tm_->getTargetTriple().getTriple());
#else
    // module->setTargetTriple(tm_->getTargetTriple());
#endif
    // Generate LLVM intermediate representation
    const auto builder = std::make_unique<LLVMIRBuilder>(config_, *context, module.get());
    builder->build(ast);
    // TODO run optimizer?
    if (module && logger_.getErrorCount() == 0) {
        logger_.debug("Running JIT...");
        exitOnErr_(jit->addIRModule(orc::ThreadSafeModule(std::move(module), std::move(context))));
        // Link with other imported modules (*.o and *.obj files)
        loadObjects(ast, jit.get());
        // Register signal handler for Oberon traps
        register_signal_handler();
        // Execute Oberon program using ORC JIT
        const string entry = ast->getTranslationUnit()->getIdentifier()->name();
        const auto mainAddr = exitOnErr_(jit->lookup(entry));
        const auto mainFn = mainAddr.toPtr<int()>();
        const int result = mainFn();
        logger_.debug("Process finished with exit code " + to_string(result));
        return result;
    }
    logger_.error(path.filename().string(), "code generation failed.");
    return EXIT_FAILURE;
}
#endif

void LLVMCodeGen::emit(Module *module, path path, OutputFileType type) const {
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
#if (defined(_WIN32) || defined(_WIN64)) && !(defined(__MINGW32__) || defined(__MINGW64__))
            ext = "obj";
#else
            ext = "o";
#endif
            break;
    }
    std::string name = config_.getOutputFile();
    if (config_.hasRunLinker() || name.empty()) {
        name = path.replace_extension(ext).string();
    }
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
    [[maybe_unused]] CodeGenFileType ft;
    switch (type) {
        case OutputFileType::AssemblyFile:
#if defined(_LLVM_18) || defined(_LLVM_19)  || defined(_LLVM_20) || defined(_LLVM_21)
            ft = CodeGenFileType::AssemblyFile;
#else
            ft = CodeGenFileType::CGFT_AssemblyFile;
#endif
            break;
        default:
#if defined(_LLVM_18) || defined(_LLVM_19)  || defined(_LLVM_20) || defined(_LLVM_21)
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
