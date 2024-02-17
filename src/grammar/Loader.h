/*
 * Utility to load a grammar from a file.
 *
 * Created by Michael Grossniklaus on 3/14/20.
 */

#ifndef OBERON_LLVM_LOADER_H
#define OBERON_LLVM_LOADER_H


#include <filesystem>

#include "scanner/Scanner.h"
#include "Grammar.h"

namespace fs = std::filesystem;

class Loader {

private:
    Logger &logger_;
    Scanner scanner_;
    unsigned int id_;

    [[nodiscard]] std::string getNextId();
    void production(Grammar *grammar);
    [[nodiscard]] Symbol * symbol(Grammar *grammar);
    void symbol_list(Grammar *grammar, std::vector<Symbol*> &symbols, const std::unordered_set<TokenType>& follows);
    [[nodiscard]] NonTerminal * non_terminal(Grammar *grammar);
    [[nodiscard]] Terminal * terminal(Grammar *grammar);
    void alternation(Grammar *grammar, NonTerminal *head, std::unordered_set<TokenType> follows);
    [[nodiscard]] NonTerminal * optional(Grammar *grammar);

public:
    Loader(CompilerConfig &config, fs::path path) : logger_(config.logger()), scanner_(config, path), id_(0) {};
    ~Loader() = default;

    [[nodiscard]] std::unique_ptr<Grammar> load();
};


#endif //OBERON_LLVM_LOADER_H
