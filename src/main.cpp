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

int main(const int argc, const char **argv) {
    auto logger = std::make_unique<Logger>(LogLevel::INFO, &std::cout);
#ifdef _DEBUG
    logger->setLevel(LogLevel::DEBUG);
#endif
    // TODO move CodeGen into Compiler by moving corresponding flags to CompilerFlags
    auto codegen = std::make_unique<LLVMCodeGen>(logger.get());
    auto flags = std::make_unique<CompilerFlags>();
    auto compiler = std::make_unique<Compiler>(logger.get(), flags.get(), codegen.get());
    auto visible = po::options_description("OPTIONS");
    visible.add_options()
            ("help,h", "Display available visible.")
            ("version", "Print version information.")
            (",I", po::value<std::string>()->value_name("<directories>"), "Search paths for symbol files.")
            (",L", po::value<std::string>()->value_name("<directories>"), "Search paths for libraries.")
            (",l", po::value<std::vector<std::string>>()->value_name("<library>"), "Static or dynamic library.")
            (",O", po::value<int>()->value_name("<level>"), "Optimization level. [O0, O1, O2, O3]")
            (",o", po::value<std::string>()->value_name("<filename>"), "Name of the output file.")
            ("filetype", po::value<std::string>()->value_name("<type>"), "Set type of output file. [asm, bc, obj, ll]")
            ("reloc", po::value<std::string>()->value_name("<model>"), "Set relocation model. [default, static, pic]")
            ("target", po::value<std::string>()->value_name("<triple>"), "Target triple for cross compilation.")
            ("run,r", "Run with LLVM JIT.")
            ("verbose,v", "Enable debugging outputs.")
            ("quiet,q", "Suppress all compiler outputs.");
    auto hidden = po::options_description("HIDDEN");
    hidden.add_options()
            ("inputs", po::value<std::vector<std::string>>());
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
        std::cout << "OVERVIEW: " << PROJECT_NAME << " LLVM compiler\n" << std::endl;
        std::cout << "USAGE: " << PROJECT_NAME << " [options] file...\n" << std::endl;
        std::cout << visible << std::endl;
        return EXIT_SUCCESS;
    } else if (vm.count("version")) {
        std::cout << PROJECT_NAME << " version " << PROJECT_VERSION << std::endl;
        std::cout << "Target: " << codegen->getDescription() << std::endl;
        return EXIT_SUCCESS;
    } else if (vm.count("inputs")) {
        if (vm.count("quiet")) {
            logger->setLevel(LogLevel::QUIET);
        }
        if (vm.count("verbose")) {
            logger->setLevel(LogLevel::DEBUG);
        }
        if (vm.count("-I")) {
            auto param = vm["-I"].as<std::string>();
            std::vector<std::string> includes;
            boost::algorithm::split(includes, param, boost::is_any_of(";"));
            for (const auto& include : includes) {
                flags->addIncludeDirectory(include);
                logger->debug(PROJECT_NAME, "adding include search path: \"" + include + "\".");
            }
        }
        if (vm.count("-L")) {
            auto param = vm["-L"].as<std::string>();
            std::vector<std::string> libraries;
            boost::algorithm::split(libraries, param, boost::is_any_of(";"));
            for (const auto& library : libraries) {
                flags->addLibraryDirectory(library);
                logger->debug(PROJECT_NAME, "adding library search path: \"" + library + "\".");
            }
        }
        if (vm.count("-l")) {
            auto param = vm["-l"].as<std::vector<std::string>>();
            for (const auto& lib : param) {
                flags->addLibrary(lib);
                logger->debug(PROJECT_NAME, "adding library: \"" + lib + "\".");
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
                    logger->error(PROJECT_NAME, "unsupported optimization level: " + std::to_string(level) + ".");
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
            auto type = vm["filetype"].as<std::string>();
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
            auto model = vm["reloc"].as<std::string>();
            if (model == "pic") {
                flags->setRelocationModel(RelocationModel::PIC);
            } else if (model == "static") {
                flags->setRelocationModel(RelocationModel::STATIC);
            } else {
                flags->setRelocationModel(RelocationModel::DEFAULT);
            }
        }
        if (vm.count("-o")) {
            flags->setOutputFile(vm["-o"].as<std::string>());
        }
        if (vm.count("target")) {
            if (flags->isJit()) {
                logger->error(PROJECT_NAME, "flag not supported in JIT mode: target");
                return EXIT_FAILURE;
            }
            flags->setTargetTriple(vm["target"].as<std::string>());
        }
        codegen->configure(flags.get());
        if (logger->getErrorCount() != 0) {
            return EXIT_FAILURE;
        }
        auto inputs = vm["inputs"].as<std::vector<std::string>>();
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
                logger->info(PROJECT_NAME, "compiling module " + input + ".");
                auto path = fs::path(input);
                compiler->compile(path);
            }
        }
        std::string status = (logger->getErrorCount() == 0 ? "complete" : "failed");
        logger->info(PROJECT_NAME, "compilation " + status + ": " +
                                   std::to_string(logger->getErrorCount()) + " error(s), " +
                                   std::to_string(logger->getWarningCount()) + " warning(s), " +
                                   std::to_string(logger->getInfoCount()) + " message(s).");
        exit(logger->getErrorCount() != 0);
    } else {
        logger->error(PROJECT_NAME, "no input files specified.");
        return EXIT_FAILURE;
    }
}