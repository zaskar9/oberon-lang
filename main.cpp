/*
 * Main class of the Oberon compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <config.h>
#include "llvm/LLVMCompiler.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(const int argc, const char *argv[]) {
    auto logger = std::make_unique<Logger>(LogLevel::INFO, &std::cout);
    logger->setLevel(LogLevel::INFO);
    auto compiler = std::make_unique<LLVMCompiler>(logger.get());
    auto visible = po::options_description("OPTIONS");
    visible.add_options()
            ("help,h", "Display available visible.")
            ("version,v", "Print version information.")
            ("filetype", po::value<std::string>()->value_name("<type>"), "Specify type of output file. [obj, ll]")
            (",O", po::value<int>()->value_name("<level>"), "Optimization level. [O0, O1, O2, O3]");
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
        std::cout << "Target: " << compiler->getTargetMachine()->getTargetTriple().str() << std::endl;
        return 1;
    } else if (vm.count("inputs")) {
        if (vm.count("-O")) {
            int level = vm["-O"].as<int>();
            switch (level) {
                case 0:
                    break;
                case 1:
                    compiler->setOptimizationLevel(PassBuilder::O1);
                    break;
                case 2:
                    compiler->setOptimizationLevel(PassBuilder::O2);
                    break;
                case 3:
                    compiler->setOptimizationLevel(PassBuilder::O3);
                    break;
                default:
                    logger->error(PROJECT_NAME, "unsupported optimization level: " + to_string(level) + ".");
                    return 1;
            }
        }
        if (vm.count("filetype")) {
            auto type = vm["filetype"].as<std::string>();
            if (type == "asm") {
                compiler->setCodeGenFileType(OutputFileType::AssemblyFile);
                logger->error(PROJECT_NAME, "asm output file type currently not supported.");
                return 1;
            } else if (type == "bc") {
                compiler->setCodeGenFileType(OutputFileType::BitCodeFile);
            } else if (type == "ll") {
                compiler->setCodeGenFileType(OutputFileType::LLVMIRFile);
            } else if (type == "obj") {
                compiler->setCodeGenFileType(OutputFileType::ObjectFile);
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
        logger->info(PROJECT_NAME, "compilation complete: " +
                                   std::to_string(logger->getErrorCount()) + " error(s), " +
                                   std::to_string(logger->getWarningCount()) + " warning(s), " +
                                   std::to_string(logger->getInfoCount()) + " message(s).");
        exit(logger->getErrorCount() != 0);
    } else {
        logger->error(PROJECT_NAME, "no input files specified.");
        return 1;
    }
}