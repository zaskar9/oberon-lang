/*
 * Main class of the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <filesystem>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <config.h>
#include "compiler/Compiler.h"
#include "compiler/CompilerConfig.h"
#include "codegen/llvm/LLVMCodeGen.h"
#include "logging/Logger.h"

// For certain modules, LLVM emmits stack protection functionality under Windows that 
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
using std::string;
using std::to_string;
using std::vector;

int main(const int argc, const char **argv) {
    // TODO move CodeGen into Compiler by moving corresponding config to CompilerConfig
    CompilerConfig config;
    Logger &logger = config.logger();
    auto codegen = make_unique<LLVMCodeGen>(config);
    Compiler compiler(config, codegen.get());
    auto visible = po::options_description("OPTIONS");
    visible.add_options()
            ("help,h", "Displays this help information.")
            ("version", "Print version information.")
            (",I", po::value<vector<string>>()->value_name("<directories>"), "Search paths for symbol files.")
            (",L", po::value<vector<string>>()->value_name("<directories>"), "Search paths for libraries.")
            (",l", po::value<vector<string>>()->value_name("<library>"), "Static or dynamic library.")
            (",f", po::value<vector<string>>()->value_name("<flag>"), "Compiler configuration flags.")
            (",O", po::value<int>()->value_name("<level>"), "Optimization level. [O0, O1, O2, O3]")
            (",o", po::value<string>()->value_name("<filename>"), "Name of the output file.")
            (",W", po::value<vector<string>>()->value_name("<option>"), "Warning configuration.")
            ("sym-dir", po::value<string>()->value_name("<directory>"), "Set output path for generated .smb files.")
            ("filetype", po::value<string>()->value_name("<type>"), "Set type of output file. [asm, bc, obj, ll]")
            ("reloc", po::value<string>()->value_name("<model>"), "Set relocation model. [default, static, pic]")
            ("target", po::value<string>()->value_name("<triple>"), "Target triple for cross compilation.")
            ("run,r", "Run with LLVM JIT.")
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
        logger.error(PROJECT_NAME, e.what());
        return EXIT_FAILURE;
    }
    po::notify(vm);
    if (vm.count("help")) {
        cout << "OVERVIEW: " << PROJECT_NAME << " LLVM compiler\n" << endl;
        cout << "USAGE: " << PROJECT_NAME << " [options] file...\n" << endl;
        cout << visible << endl;
        return EXIT_SUCCESS;
    } else if (vm.count("version")) {
        cout << PROJECT_NAME << " version " << PROJECT_VERSION << endl;
        cout << "Target:   " << codegen->getDescription() << endl;
        cout << "Includes: ";
        cout << "Boost " << BOOST_VERSION / 100000 << "."
                              << BOOST_VERSION / 100 % 1000 << "."
                              << BOOST_VERSION % 100 << ", ";
        cout << "LLVM " << LLVM_VERSION << endl;
        return EXIT_SUCCESS;
    } else if (vm.count("inputs")) {
        if (vm.count("quiet")) {
            logger.setLevel(LogLevel::QUIET);
        }
        if (vm.count("verbose")) {
            logger.setLevel(LogLevel::DEBUG);
        }
#if (defined(_WIN32) || defined(_WIN64)) && !defined(__MINGW32__)
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
            auto params = vm["-l"].as<vector<string>>();
            for (const auto& lib : params) {
                config.addLibrary(lib);
                logger.debug("Library: '" + lib + "'.");
            }
        }
        if (vm.count("-f")) {
            auto params = vm["-f"].as<vector<string>>();
            for (const auto& flag : params) {
                if (flag == "enable-extern") {
                    config.setFlag(Flag::ENABLE_EXTERN);
                } else if (flag == "enable-varargs") {
                    config.setFlag(Flag::ENABLE_VARARGS);
                } else if (flag == "enable-main") {
                    config.setFlag(Flag::ENABLE_MAIN);
                } else if (flag == "enable-bound-checks") {
                    config.setFlag(Flag::ENABLE_BOUND_CHECKS);
                } else if (flag == "no-stack-protector") {
                    config.setFlag(Flag::NO_STACK_PROTECT);
                } else {
                    logger.warning(PROJECT_NAME, "ignoring unrecognized flag -f" + flag + ".");
                }
            }
        }
        if (vm.count("-O")) {
            int level = vm["-O"].as<int>();
            switch (level) {
                case 0:
                    config.setOptimizationLevel(::OptimizationLevel::O0);
                    break;
                case 1:
                    config.setOptimizationLevel(::OptimizationLevel::O1);
                    break;
                case 2:
                    config.setOptimizationLevel(::OptimizationLevel::O2);
                    break;
                case 3:
                    config.setOptimizationLevel(::OptimizationLevel::O3);
                    break;
                default:
                    logger.error(PROJECT_NAME, "unsupported optimization level: " + to_string(level) + ".");
                    return EXIT_FAILURE;
            }
        }
        if (vm.count("-o")) {
            config.setOutputFile(vm["-o"].as<string>());
        }
        if (vm.count("-W")) {
            auto params = vm["-W"].as<vector<string>>();
            for (const auto& warn : params) {
                if (warn == "error") {
                    config.setWarning(Warning::ERROR);
                    logger.setWarnAsError(true);
                } else {
                    logger.warning(PROJECT_NAME, "ignoring unrecognized warning -W" + warn + ".");
                }
            }
        }
        if (vm.count("run")) {
            config.setJit(true);
        }
        if (vm.count("filetype")) {
            if (config.isJit()) {
                logger.error(PROJECT_NAME, "flag not supported in JIT mode: filetype");
                return EXIT_FAILURE;
            }
            auto type = vm["filetype"].as<string>();
            if (type == "asm") {
                config.setFileType(OutputFileType::AssemblyFile);
            } else if (type == "bc") {
                config.setFileType(OutputFileType::BitCodeFile);
            } else if (type == "ll") {
                config.setFileType(OutputFileType::LLVMIRFile);
            } else if (type == "obj") {
                config.setFileType(OutputFileType::ObjectFile);
            } else {
                logger.error(PROJECT_NAME, "unsupported output file type: " + type + ".");
                return EXIT_FAILURE;
            }
        }
        if (vm.count("reloc")) {
            if (config.isJit()) {
                logger.error(PROJECT_NAME, "flag not supported int JIT mode: reloc");
                return EXIT_FAILURE;
            }
            auto model = vm["reloc"].as<string>();
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
            auto path = std::filesystem::path(config.getSymDir());
            if (!std::filesystem::is_directory(path)) {
                logger.error(PROJECT_NAME, "sym-dir path not valid");
            }
        }
        if (vm.count("target")) {
            if (config.isJit()) {
                logger.error(PROJECT_NAME, "flag not supported in JIT mode: target");
                return EXIT_FAILURE;
            }
            config.setTargetTriple(vm["target"].as<string>());
        }
        codegen->configure();
        if (logger.getErrorCount() != 0) {
            return EXIT_FAILURE;
        }
        auto inputs = vm["inputs"].as<vector<string>>();
        if (config.isJit()) {
#ifndef _LLVM_LEGACY
            if (inputs.size() != 1) {
                logger.error(PROJECT_NAME, "only one input module supported in JIT mode.");
                return EXIT_FAILURE;
            }
            auto path = std::filesystem::path(inputs[0]);
            exit(compiler.jit(path));
#else
            logger.error(PROJECT_NAME, "linked LLVM version does not support JIT mode.");
#endif
        } else {
            for (auto &input : inputs) {
                logger.debug("Compiling module " + input + ".");
                auto path = std::filesystem::path(input);
                compiler.compile(path);
            }
        }
        string status = (logger.getErrorCount() == 0 ? "complete" : "failed");
        logger.info("Compilation " + status + ": " +
                          to_string(logger.getErrorCount()) + " error(s), " +
                          to_string(logger.getWarningCount()) + " warning(s), " +
                          to_string(logger.getInfoCount()) + " message(s).");
        exit(logger.getErrorCount() != 0);
    } else {
        logger.error(PROJECT_NAME, "no input files specified.");
        return EXIT_FAILURE;
    }
}