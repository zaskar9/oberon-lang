/*
 * Main class of the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */


#include <config.h>
#include "compiler/CompilerFlags.h"
#include "compiler/Compiler.h"
#include "logging/Logger.h"
#include "codegen/llvm/LLVMCodeGen.h"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <iostream>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

using std::cout;
using std::cerr;
using std::endl;
using std::make_unique;
using std::string;
using std::to_string;
using std::vector;

int main(const int argc, const char **argv) {
    auto logger = make_unique<Logger>(LogLevel::INFO, &cout);
#ifdef _DEBUG
    logger->setLevel(LogLevel::DEBUG);
#endif
    // TODO move CodeGen into Compiler by moving corresponding flags to CompilerFlags
    auto flags = make_unique<CompilerFlags>();
    auto codegen = make_unique<LLVMCodeGen>(flags.get(), logger.get());
    auto compiler = make_unique<Compiler>(flags.get(), logger.get(), codegen.get());
    auto visible = po::options_description("OPTIONS");
    visible.add_options()
            ("help,h", "Displays this help information.")
            ("version", "Print version information.")
            (",I", po::value<vector<string>>()->value_name("<directories>"), "Search paths for symbol files.")
            (",L", po::value<vector<string>>()->value_name("<directories>"), "Search paths for libraries.")
            (",l", po::value<vector<string>>()->value_name("<library>"), "Static or dynamic library.")
            (",f", po::value<vector<string>>()->value_name("<flag>"), "Compiler flags.")
            (",O", po::value<int>()->value_name("<level>"), "Optimization level. [O0, O1, O2, O3]")
            (",o", po::value<string>()->value_name("<filename>"), "Name of the output file.")
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
        logger->error(PROJECT_NAME, e.what());
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
            logger->setLevel(LogLevel::QUIET);
        }
        if (vm.count("verbose")) {
            logger->setLevel(LogLevel::DEBUG);
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
                boost::algorithm::split(includes, param, boost::is_any_of(":"));
                for (const auto& include: includes) {
                    flags->addIncludeDirectory(include);
                    logger->debug("Include search path: '" + include + "'.");
                }
            }
        }
        if (vm.count("-L")) {
            auto params = vm["-L"].as<vector<string>>();
            vector<string> libraries;
            for (const auto& param : params) {
                boost::algorithm::split(libraries, param, boost::is_any_of(":"));
                for (const auto& library: libraries) {
                    flags->addLibraryDirectory(library);
                    logger->debug("Library search path: '" + library + "'.");
                }
            }
        }
        if (vm.count("-l")) {
            auto params = vm["-l"].as<vector<string>>();
            for (const auto& lib : params) {
                flags->addLibrary(lib);
                logger->debug("Library: '" + lib + "'.");
            }
        }
        if (vm.count("-f")) {
            auto params = vm["-f"].as<vector<string>>();
            for (const auto& flag : params) {
                if (flag == "enable-extern") {
                    flags->setFlag(Flag::ENABLE_EXTERN);
                } else if (flag == "enable-varargs") {
                    flags->setFlag(Flag::ENABLE_VARARGS);
                } else if (flag == "enable-main") {
                    flags->setFlag(Flag::ENABLE_MAIN);
                } else {
                    logger->warning(PROJECT_NAME, "ignoring unrecognized flag -f" + flag + ".");
                }
            }
        }
        if (vm.count("-O")) {
            int level = vm["-O"].as<int>();
            switch (level) {
                case 0:
                    flags->setOptimizationLevel(::OptimizationLevel::O0);
                    break;
                case 1:
                    flags->setOptimizationLevel(::OptimizationLevel::O1);
                    break;
                case 2:
                    flags->setOptimizationLevel(::OptimizationLevel::O2);
                    break;
                case 3:
                    flags->setOptimizationLevel(::OptimizationLevel::O3);
                    break;
                default:
                    logger->error(PROJECT_NAME, "unsupported optimization level: " + to_string(level) + ".");
                    return EXIT_FAILURE;
            }
        }
        if (vm.count("run")) {
            flags->setJit(true);
        }
        if (vm.count("filetype")) {
            if (flags->isJit()) {
                logger->error(PROJECT_NAME, "flag not supported in JIT mode: filetype");
                return EXIT_FAILURE;
            }
            auto type = vm["filetype"].as<string>();
            if (type == "asm") {
                flags->setFileType(OutputFileType::AssemblyFile);
            } else if (type == "bc") {
                flags->setFileType(OutputFileType::BitCodeFile);
            } else if (type == "ll") {
                flags->setFileType(OutputFileType::LLVMIRFile);
            } else if (type == "obj") {
                flags->setFileType(OutputFileType::ObjectFile);
            } else {
                logger->error(PROJECT_NAME, "unsupported output file type: " + type + ".");
                return EXIT_FAILURE;
            }
        }
        if (vm.count("reloc")) {
            if (flags->isJit()) {
                logger->error(PROJECT_NAME, "flag not supported int JIT mode: reloc");
                return EXIT_FAILURE;
            }
            auto model = vm["reloc"].as<string>();
            if (model == "pic") {
                flags->setRelocationModel(RelocationModel::PIC);
            } else if (model == "static") {
                flags->setRelocationModel(RelocationModel::STATIC);
            } else {
                flags->setRelocationModel(RelocationModel::DEFAULT);
            }
        }
        if (vm.count("-o")) {
            flags->setOutputFile(vm["-o"].as<string>());
        }
        if (vm.count("target")) {
            if (flags->isJit()) {
                logger->error(PROJECT_NAME, "flag not supported in JIT mode: target");
                return EXIT_FAILURE;
            }
            flags->setTargetTriple(vm["target"].as<string>());
        }
        codegen->configure(flags.get());
        if (logger->getErrorCount() != 0) {
            return EXIT_FAILURE;
        }
        auto inputs = vm["inputs"].as<vector<string>>();
        if (flags->isJit()) {
#ifndef _LLVM_LEGACY
            if (inputs.size() != 1) {
                logger->error(PROJECT_NAME, "only one input module supported in JIT mode.");
                return EXIT_FAILURE;
            }
            auto path = fs::path(inputs[0]);
            exit(compiler->jit(path));
#else
            logger->error(PROJECT_NAME, "linked LLVM version does not support JIT mode.");
#endif
        } else {
            for (auto &input : inputs) {
                logger->debug("Compiling module " + input + ".");
                auto path = fs::path(input);
                compiler->compile(path);
            }
        }
        string status = (logger->getErrorCount() == 0 ? "complete" : "failed");
        logger->info("Compilation " + status + ": " +
                          to_string(logger->getErrorCount()) + " error(s), " +
                          to_string(logger->getWarningCount()) + " warning(s), " +
                          to_string(logger->getInfoCount()) + " message(s).");
        exit(logger->getErrorCount() != 0);
    } else {
        logger->error(PROJECT_NAME, "no input files specified.");
        return EXIT_FAILURE;
    }
}