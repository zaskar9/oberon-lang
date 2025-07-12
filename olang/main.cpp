/*
 * Main class of the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <filesystem>
#include <iostream>
#include <regex>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "config.h"
#include "Logger.h"
#include "codegen/llvm/LLVMCodeGen.h"
#include "compiler/Compiler.h"
#include "compiler/CompilerConfig.h"

// For certain modules, LLVM emits stack protection functionality under Windows that
// involves calls to the standard runtime of the target platform. Since these libraries 
// are not loaded into the JIT environment by default, running these modules in JIT 
// mode will fail. As a work-around, the following pragma directives tell the linker to
// re-export these symbols.
#if (defined(_WIN32) || defined(_WIN64)) 
    #if defined(__clang__)
    #pragma comment(linker, "/export:___chkstk_ms")
    #elif defined(__GNUC__) || defined(__GNUG__)
    #pragma comment(linker, "/export:__chkstk_ms")
    #elif defined(_MSC_VER)
    #pragma comment(linker, "/export:__security_cookie")
    #pragma comment(linker, "/export:__security_check_cookie")
    #endif
#endif

namespace po = boost::program_options;

using std::cout;
using std::cerr;
using std::endl;
using std::make_unique;
using std::regex;
using std::regex_search;
using std::smatch;
using std::string;
using std::to_string;
using std::vector;

int main(const int argc, const char **argv) {
    // TODO move CodeGen into Compiler by moving corresponding config to CompilerConfig
    CompilerConfig config;
    Logger &logger = config.logger();
    logger.setBanner(PROGRAM_NAME);
    auto codegen = make_unique<LLVMCodeGen>(config);
    Compiler compiler(config, codegen.get());
    auto visible = po::options_description("OPTIONS");
    visible.add_options()
            ("help,h", "Displays this help information.")
            ("version", "Print version information.")
            (",c", "Only run compile and assemble steps.")
            ("emit-llvm", "Use the LLVM representation for assembler and object files.")
            (",I", po::value<vector<string>>()->value_name("<directories>"), "Search paths for symbol files.")
            (",L", po::value<vector<string>>()->value_name("<directories>"), "Search paths for libraries.")
            (",l", po::value<vector<string>>()->value_name("<library>"), "Static or dynamic library.")
            (",f", po::value<vector<string>>()->value_name("<flag>"), "Compiler configuration flags.")
            (",O", po::value<int>()->value_name("<level>"), "Optimization level. [O0, O1, O2, O3]")
            (",o", po::value<string>()->value_name("<filename>"), "Name of the output file.")
            (",W", po::value<vector<string>>()->value_name("<option>"), "Warning configuration.")
            ("sym-dir", po::value<string>()->value_name("<directory>"), "Set output path for generated .smb files.")
            // ("filetype", po::value<string>()->value_name("<type>"), "Set type of output file. [asm, bc, obj, ll]")
            ("reloc", po::value<string>()->value_name("<model>"), "Set relocation model. [default, static, pic]")
            ("target", po::value<string>()->value_name("<triple>"), "Target triple for cross compilation.")
            ("run,r", "Run with LLVM JIT.")
            (",S", "Only run compilation steps.")
            ("verbose,v", "Enable debugging outputs.")
            ("quiet,q", "Suppress all compiler outputs.");
    auto hidden = po::options_description("HIDDEN");
    hidden.add_options()
            ("inputs", po::value<vector<string>>());
    po::positional_options_description p;
    p.add("inputs", -1);
    po::options_description all;
    all.add(visible).add(hidden);
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv)
            .options(all)
            .positional(p)
            .run(),
        vm);
    } catch (po::error &e) {
        logger.error(PROGRAM_NAME, e.what());
        return EXIT_FAILURE;
    }
    po::notify(vm);
    if (vm.count("help")) {
        cout << "OVERVIEW: " << PROGRAM_NAME << " LLVM compiler\n" << endl;
        cout << "USAGE: " << PROGRAM_NAME << " [options] file...\n" << endl;
        cout << visible << endl;
        return EXIT_SUCCESS;
    }
    if (vm.count("version")) {
        cout << PROGRAM_NAME << " version " << PROJECT_VERSION;
        cout << " (build " << GIT_COMMIT << "@" << GIT_BRANCH << ")" << endl;
        cout << "Target:   " << codegen->getDescription() << endl;
        cout << "Includes: ";
        cout << "Boost " << BOOST_VERSION / 100000 << "."
                << BOOST_VERSION / 100 % 1000 << "."
                << BOOST_VERSION % 100 << ", ";
        cout << "LLVM " << LLVM_VERSION << endl;
        return EXIT_SUCCESS;
    }
    if (vm.count("inputs")) {
        if (vm.count("quiet")) {
            logger.setLevel(LogLevel::QUIET);
        }
        if (vm.count("verbose")) {
            logger.setLevel(LogLevel::DEBUG);
        }
#if (defined(_WIN32) || defined(_WIN64)) && !(defined(__MINGW32__) || defined(__MINGW64__))
        // Windows uses a semicolon to separate multiple paths
        std::string separator = ";";
#else
        std::string separator = ":";
#endif
        if (vm.count("-I")) {
            auto params = vm["-I"].as<vector<string>>();
            vector<string> includes;
            for (const auto& param : params) {
                boost::algorithm::split(includes, param, boost::is_any_of(separator));
                for (const auto& include: includes) {
                    config.addIncludeDirectory(include);
                    logger.debug("Include search path: '" + include + "'.");
                }
            }
        }
        if (vm.count("-L")) {
            auto params = vm["-L"].as<vector<string>>();
            vector<string> libraries;
            for (const auto& param : params) {
                boost::algorithm::split(libraries, param, boost::is_any_of(separator));
                for (const auto& library: libraries) {
                    config.addLibraryDirectory(library);
                    logger.debug("Library search path: '" + library + "'.");
                }
            }
        }
        if (vm.count("-l")) {
            const auto params = vm["-l"].as<vector<string>>();
            for (const auto& lib : params) {
                config.addLibrary(lib);
                logger.debug("Library: '" + lib + "'.");
            }
        }
        if (vm.count("-f")) {
            auto params = vm["-f"].as<vector<string>>();
            regex pattern{"^(no-)?sanitize=(.*)$"};
            for (const auto& flag : params) {
                if (flag == "enable-extern") {
                    config.setFlag(Flag::ENABLE_EXTERN);
                } else if (flag == "enable-varargs") {
                    config.setFlag(Flag::ENABLE_VARARGS);
                } else if (flag == "enable-main") {
                    config.setFlag(Flag::ENABLE_MAIN);
                } else if (flag == "no-stack-protector") {
                    config.setFlag(Flag::NO_STACK_PROTECT);
                } else if (smatch matches; regex_search(flag, matches, pattern)) {
                    bool active = flag[0] != 'n';
                    const string opt = matches.str(2);
                    if (opt == "array-bounds" || opt == "bounds") {
                        // Out of bounds array indexing.
                        config.toggleSanitize(Trap::OUT_OF_BOUNDS, active);
                    } else if (opt == "type-guard") {
                        // Using an invalid type as guard will lead to undefined behaviour.
                        config.toggleSanitize(Trap::TYPE_GUARD, active);
                    } else if (opt == "copy-overflow") {
                        // Assignment of an array or a string to a variable that is not large enough to
                        // hold the value.
                        config.toggleSanitize(Trap::COPY_OVERFLOW, active);
                    } else if (opt == "float-divide-by-zero") {
                        // Floating point division by zero.
                        config.toggleSanitize(Trap::FLT_DIVISION, active);
                    } else if (opt == "function" || opt == "procedure") {
                        // Indirect call of a procedure through a pointer of the wrong type.
                        config.toggleSanitize(Trap::PROCEDURE_CALL, active);
                    } else if (opt == "integer-divide-by-zero") {
                        // Integer division by zero.
                        config.toggleSanitize(Trap::INT_DIVISION, active);
                    } else if (opt == "assert") {
                        // Toggle whether assert leads to a trap or to a crash.
                        config.toggleSanitize(Trap::ASSERT, active);
                    } else if (opt == "null" || opt == "nil") {
                        // Use of a null pointer or creation of a null reference.
                        config.toggleSanitize(Trap::NIL_POINTER, active);
                    } else if (opt == "signed-integer-overflow") {
                        // Signed integer overflow, where the result of a signed integer computation
                        // cannot be represented in its type.
                        config.toggleSanitize(Trap::INT_OVERFLOW, active);
                    } else if (opt == "sign-conversion") {
                        // Implicit conversions of signed to unsigned integers can lead to undefined behavior.
                        config.toggleSanitize(Trap::SIGN_CONVERSION, active);
                    } else if (opt == "undefined") {
                        config.toggleSanitize(Trap::OUT_OF_BOUNDS, active);
                        config.toggleSanitize(Trap::COPY_OVERFLOW, active);
                        config.toggleSanitize(Trap::FLT_DIVISION, active);
                        config.toggleSanitize(Trap::INT_DIVISION, active);
                        config.toggleSanitize(Trap::INT_OVERFLOW, active);
                        config.toggleSanitize(Trap::SIGN_CONVERSION, active);
                        config.toggleSanitize(Trap::TYPE_GUARD, active);
                        config.toggleSanitize(Trap::PROCEDURE_CALL, active);
                    } else if (opt == "all") {
                        if (active) {
                            config.setSanitizeAll();
                        } else {
                            config.setSanitizeNone();
                        }
                    } else {
                        logger.warning(PROGRAM_NAME, "ignoring unrecognized option -f" + flag + ".");
                    }
                } else {
                    logger.warning(PROGRAM_NAME, "ignoring unrecognized option -f" + flag + ".");
                }
            }
        }
        if (vm.count("-O")) {
            switch (int level = vm["-O"].as<int>()) {
                case 0:
                    config.setOptimizationLevel(OptimizationLevel::O0);
                    break;
                case 1:
                    config.setOptimizationLevel(OptimizationLevel::O1);
                    break;
                case 2:
                    config.setOptimizationLevel(OptimizationLevel::O2);
                    break;
                case 3:
                    config.setOptimizationLevel(OptimizationLevel::O3);
                    break;
                default:
                    logger.error(PROGRAM_NAME, "unsupported optimization level: " + to_string(level) + ".");
                    return EXIT_FAILURE;
            }
        }
        if (vm.count("-o")) {
            config.setOutputFile(vm["-o"].as<string>());
        }
        if (vm.count("-W")) {
            const auto params = vm["-W"].as<vector<string>>();
            for (const auto& warn : params) {
                if (warn == "error") {
                    config.setWarning(Warning::ERROR);
                    logger.setWarnAsError(true);
                } else {
                    logger.warning(PROGRAM_NAME, "ignoring unrecognized warning -W" + warn + ".");
                }
            }
        }
        if (vm.count("run")) {  // run
            config.setJit(true);
        } else if (vm.count("-c")) {  // compile and assemble
            if (vm.count("emit-llvm")) {
                config.setFileType(OutputFileType::BitCodeFile);
            } else {
                config.setFileType(OutputFileType::ObjectFile);
            }
        } else if (vm.count("-S")) {  // assemble
            if (vm.count("emit-llvm")) {
                config.setFileType(OutputFileType::LLVMIRFile);
            } else {
                config.setFileType(OutputFileType::AssemblyFile);
            }
        } else {
            if (vm.count("emit-llvm")) {
                logger.error(PROGRAM_NAME, "--emit-llvm cannot be used when linking.");
            }
            logger.error(PROGRAM_NAME, "linking not yet supported.");
            return EXIT_FAILURE;
        }
        if (vm.count("reloc")) {
            if (config.isJit()) {
                logger.error(PROGRAM_NAME, "--reloc not compatible with --run.");
                return EXIT_FAILURE;
            }
            const auto model = vm["reloc"].as<string>();
            if (model == "pic") {
                config.setRelocationModel(RelocationModel::PIC);
            } else if (model == "static") {
                config.setRelocationModel(RelocationModel::STATIC);
            } else {
                config.setRelocationModel(RelocationModel::DEFAULT);
            }
        }
        if (vm.count("sym-dir")) {
            config.setSymDir(vm["sym-dir"].as<string>());
            const auto path = std::filesystem::path(config.getSymDir());
            if (!std::filesystem::is_directory(path)) {
                logger.error(PROGRAM_NAME, "--sym-dir path not valid.");
            }
        }
        if (vm.count("target")) {
            if (config.isJit()) {
                logger.error(PROGRAM_NAME, "--target not supported in JIT mode.");
                return EXIT_FAILURE;
            }
            config.setTargetTriple(vm["target"].as<string>());
        }
        codegen->configure();
        if (logger.getErrorCount() != 0) {
            return EXIT_FAILURE;
        }
        auto &inputs = vm["inputs"].as<vector<string>>();
        if (config.isJit()) {
#ifndef _LLVM_LEGACY
            if (inputs.size() != 1) {
                logger.error(PROGRAM_NAME, "--run requires exactly one input module.");
                return EXIT_FAILURE;
            }
            auto path = std::filesystem::path(inputs[0]);
            exit(compiler.jit(path));
#else
            logger.error(PROGRAM_NAME, "linked LLVM version does not support JIT mode.");
            return EXIT_FAILURE;
#endif
        }
        for (auto &input : inputs) {
            logger.debug("Compiling module " + input + ".");
            auto path = std::filesystem::path(input);
            compiler.compile(path);
        }
        string status = logger.getErrorCount() == 0 ? "complete" : "failed";
        logger.info("Compilation " + status + ": " +
                    to_string(logger.getErrorCount()) + " error(s), " +
                    to_string(logger.getWarningCount()) + " warning(s), " +
                    to_string(logger.getInfoCount()) + " message(s).");
        exit(logger.getErrorCount() != 0);
    }
    logger.error(PROGRAM_NAME, "no input files specified.");
    return EXIT_FAILURE;
}
