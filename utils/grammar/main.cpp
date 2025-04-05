/*
 * Main of the utility to compute the FIRST, FIRST+, and FOLLOW sets of a context-free grammar.
 *
 * Created by Michael Grossniklaus on 3/11/20.
 */


#include <filesystem>
#include <iostream>
#include <iomanip>

#include <config.h>
#include "Loader.h"
#include "Validator.h"

namespace fs = std::filesystem;

int main(const int argc, const char *argv[]) {
    Logger logger;
    logger.setBanner(PROGRAM_NAME);
    logger.setLevel(LogLevel::INFO);
    if (argc < 2) {
        logger.error(PROGRAM_NAME, "Too few arguments.");
        exit(1);
    }
    Loader loader(logger, fs::path(argv[1]));
    auto grammar = loader.load();
    std::cout << *grammar << std::endl;

    Validator util(grammar.get());
    auto firsts = util.computeFirstSets();
    for (auto &entry: firsts) {
        std::cout << std::left << std::setw(25) << *entry.first << " | [";
        std::string sep;
        for (auto &symbol: entry.second) {
            std::cout << sep << *symbol;
            sep = ", ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;
    auto follows = util.computeFollowSets(firsts);
    for (auto &entry: follows) {
        std::cout << std::left << std::setw(25) << *entry.first << " | [";
        std::string sep;
        for (auto &symbol: entry.second) {
            std::cout << sep << *symbol;
            sep = ", ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;
    auto first_pluses = util.computeFirstPlusSets(firsts, follows);
    for (auto &entry: first_pluses) {
        std::cout << std::left << std::setw(50) << *entry.first << " | [";
        std::string sep;
        for (auto &symbol: entry.second) {
            std::cout << sep << *symbol;
            sep = ", ";
        }
        std::cout << "]" << std::endl;
    }
    std::cout << std::endl;
    std::pair<Production *, Production *> pair;
    if (!util.checkBacktrackFree(first_pluses, pair)) {
        std::cerr << *pair.first << " <=> " << *pair.second << std::endl;
    } else {
        std::cout << "Grammar is backtrack-free." << std::endl;
    }
    return 0;
}