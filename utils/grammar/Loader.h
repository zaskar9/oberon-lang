/*
 * Utility to load a grammar from a file.
 *
 * Created by Michael Grossniklaus on 3/14/20.
 */

#ifndef OBERON_LLVM_LOADER_H
#define OBERON_LLVM_LOADER_H


#include <filesystem>

#include "Grammar.h"
#include "Scanner.h"

namespace fs = std::filesystem;

class Loader {

public:
    Loader(Logger &logger, const path& path) : logger_(logger), scanner_(logger, path), id_(0) {}
    ~Loader() = default;

    [[nodiscard]] std::unique_ptr<Grammar> load();

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
    // [[nodiscard]] NonTerminal * optional(Grammar *grammar);

};


#endif //OBERON_LLVM_LOADER_H
