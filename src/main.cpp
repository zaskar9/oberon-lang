/*
 * Main class of the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <config.h>
#include "codegen/llvm/LLVMCodeGen.h"
#include "compiler/CompilerFlags.h"
#include "compiler/Compiler.h"
#include "logging/Logger.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(const int argc, const char *argv[]) {
    auto logger = std::make_unique<Logger>(LogLevel::INFO, &std::cout);
#ifdef _DEBUG
    logger->setLevel(LogLevel::DEBUG);
#endif
    // TODO move CodeGen into Compiler by moving corresponding flags to CompilerFlags
    auto codegen = std::make_unique<LLVMCodeGen>(logger.get());
    auto flags = std::make_unique<CompilerFlags>();
    auto compiler = std::make_unique<Compiler>(logger.get(), codegen.get());
    auto visible = po::options_description("OPTIONS");
    visible.add_options()
            ("help,h", "Display available visible.")
            ("version,v", "Print version information.")
            ("filetype", po::value<std::string>()->value_name("<type>"), "Specify type of output file. [asm, bc, obj, ll]")
            (",O", po::value<int>()->value_name("<level>"), "Optimization level. [O0, O1, O2, O3]")
            ("quiet,q", "Suppress all compiler outputs.")
            (",I", po::value<std::string>()->value_name("<directories>"), "Include directories for symbol files.");
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
        return 1;
    }
    po::notify(vm);
    if (vm.count("help")) {
        std::cout << "OVERVIEW: " << PROJECT_NAME << " LLVM compiler\n" << std::endl;
        std::cout << "USAGE: " << PROJECT_NAME << " [options] file...\n" << std::endl;
        std::cout << visible << std::endl;
        return 1;
    } else if (vm.count("version")) {
        std::cout << PROJECT_NAME << " version " << PROJECT_VERSION << std::endl;
        std::cout << "Target: " << codegen->getDescription() << std::endl;
        return 1;
    } else if (vm.count("inputs")) {
        if (vm.count("quiet")) {
            logger->setLevel(LogLevel::QUIET);
        }
        if (vm.count("-I")) {
            auto param = vm["-I"].as<std::string>();
            std::vector<std::string> includes;
            boost::algorithm::split(includes, param, boost::is_any_of(";"));
            for (auto include: includes) {
                flags->addInclude(include);
                logger->debug(PROJECT_NAME, "adding include directory: \"" + include + "\".");
            }
        }
        if (vm.count("-O")) {
            int level = vm["-O"].as<int>();
            switch (level) {
                case 0:
                    codegen->setOptimizationLevel(::OptimizationLevel::O0);
                    break;
                case 1:
                    codegen->setOptimizationLevel(::OptimizationLevel::O1);
                    break;
                case 2:
                    codegen->setOptimizationLevel(::OptimizationLevel::O2);
                    break;
                case 3:
                    codegen->setOptimizationLevel(::OptimizationLevel::O3);
                    break;
                default:
                    logger->error(PROJECT_NAME, "unsupported optimization level: " + to_string(level) + ".");
                    return 1;
            }
        }
        if (vm.count("filetype")) {
            auto type = vm["filetype"].as<std::string>();
            if (type == "asm") {
                codegen->setFileType(OutputFileType::AssemblyFile);
            } else if (type == "bc") {
                codegen->setFileType(OutputFileType::BitCodeFile);
            } else if (type == "ll") {
                codegen->setFileType(OutputFileType::LLVMIRFile);
            } else if (type == "obj") {
                codegen->setFileType(OutputFileType::ObjectFile);
            } else {
                logger->error(PROJECT_NAME, "unsupported output file type: " + type + ".");
                return 1;
            }
        }
        auto inputs = vm["inputs"].as<std::vector<std::string>>();
        for (auto &input : inputs) {
            logger->info(PROJECT_NAME, "compiling module " + input + ".");
            auto path = fs::path(input);
            compiler->compile(path);
        }
        std::string status = (logger->getErrorCount() == 0 ? "complete" : "failed");
        logger->info(PROJECT_NAME, "compilation " + status + ": " +
                                   std::to_string(logger->getErrorCount()) + " error(s), " +
                                   std::to_string(logger->getWarningCount()) + " warning(s), " +
                                   std::to_string(logger->getInfoCount()) + " message(s).");
        exit(logger->getErrorCount() != 0);
    } else {
        logger->error(PROJECT_NAME, "no input files specified.");
        return 1;
    }
}