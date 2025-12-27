//
// Created by Michael Grossniklaus on 4/3/25.
//

#include "LLDWrapper.h"

#include <string>
#include <vector>
#include <lld/Common/Driver.h>
#include <lld/Common/ErrorHandler.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>

LLD_HAS_DRIVER(coff)
LLD_HAS_DRIVER(elf)
LLD_HAS_DRIVER(mingw)
LLD_HAS_DRIVER(macho)
LLD_HAS_DRIVER(wasm)

using llvm::ArrayRef;
using llvm::Triple;
using std::string;
using std::vector;

int LLDWrapper::link() const {
    string triple = config_.getTargetTriple();
    if (triple.empty()) {
        triple = llvm::sys::getDefaultTargetTriple();
    }
    vector<string> opts;
    // Configure name of the linker "binary", required to correctly interface with lld
    opts.emplace_back("oberon-lld");
    // Configure lld options that can be inferred from target triple
    parseTriple(triple, opts);
    // Configure name of output binary
    opts.emplace_back("-o");
    string output = config_.getOutputFile();
    output = output.empty() ? "a.out" : output;
    opts.emplace_back(output);
    // Configure library paths
    for (const auto& directory: config_.getLibraryDirectories()) {
        if (std::filesystem::exists(directory)) {
            opts.emplace_back("-L" + directory.string());
        }
    }
    // Configure libraries
    for (const auto& library : config_.getLibraries()) {
        opts.emplace_back("-l" + library);
    }
    // Configure input files
    const auto t = Triple(triple);
    const string obj = t.isOSWindows() && !t.isOSCygMing() ? ".obj" : ".o";
    for (const auto& input: config_.getInputFiles()) {
        const string ext = input.extension().string();
        if (ext == ".Mod" || ext == ".mod") {
            string file = input.filename().string();
            string stem = file.substr(0, file.find_last_of('.'));
            opts.emplace_back(stem + obj);
        }
    }
#ifdef _DEBUG
    for (const auto& opt: opts) {
        std::cout << opt << " ";
    }
    std::cout << std::endl;
#endif
    // Convert vector<string> to vector<const char*>
    vector<const char*> argv;
    argv.reserve(opts.size());
    for(const auto& opt : opts) {
        argv.push_back(opt.c_str());
    }
    // Configure and call lld
    std::string outs_string {};
    std::string errs_string {};
    llvm::raw_string_ostream outs { outs_string };
    llvm::raw_string_ostream errs { errs_string };
    const ArrayRef<const char*> args(argv.data(), argv.size());
    auto [retCode, canRunAgain] = lld::lldMain(args, llvm::outs(),
                                               llvm::errs(), LLD_ALL_DRIVERS);
    outs.flush();
    errs.flush();
    return retCode;
}

void LLDWrapper::parseTriple(const string& triple, vector<string>& opts) const {
    string flavor;
    const auto t = Triple(triple);
    if (t.isOSBinFormatELF()) {
        flavor = "ld.lld";
        opts.emplace_back("-flavor");
        opts.emplace_back(flavor);
        opts.emplace_back("-e");
        opts.emplace_back("main");
        return;
    } else if (t.isOSBinFormatMachO()) {
        flavor = "ld64.lld";
        opts.emplace_back("-flavor");
        opts.emplace_back(flavor);
        opts.emplace_back("-arch");
        opts.emplace_back(t.getArchName());
        if (t.isMacOSX()) {
            llvm::VersionTuple version;
            t.getMacOSXVersion(version);
            opts.emplace_back("-platform_version");
            opts.emplace_back("macos");
            opts.emplace_back(to_string(version.getMajor()));
            opts.emplace_back("0");  // TODO: check what this is
        } else {
            logger_.error("lld", "unknown platform version.");
        }
        return;
    } else if (t.isOSBinFormatCOFF()) {
        flavor = "lld-link";
    } else if (t.isOSBinFormatWasm()) {
        flavor = "wasm-ld";
    }
    if (!flavor.empty()) {
        opts.emplace_back("-flavor");
        opts.emplace_back(flavor);
    } else {
        logger_.error("lld", "unknown binary format.");
    }
}

