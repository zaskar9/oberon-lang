/*
 * Utility to load a grammar from a file.
 *
 * Created by Michael Grossniklaus on 3/14/20.
 */

#ifndef OBERON_LLVM_LOADER_H
#define OBERON_LLVM_LOADER_H


#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

#include "Grammar.h"
#include "Scanner.h"

using std::filesystem::path;
using std::string;
using std::unordered_set;
using std::vector;

class Loader {

public:
    Loader(Logger &logger, const path& path) : logger_(logger), scanner_(logger, path), id_(0) {}
    ~Loader() = default;

    [[nodiscard]] std::unique_ptr<Grammar> load();

private:
    Logger &logger_;
    Scanner scanner_;
    unsigned int id_;
    NonTerminal *head_;

    void production(Grammar *);
    void alternation(Grammar *, NonTerminal *, std::unordered_set<TokenType>);
    void symbol_list(Grammar *, std::vector<Symbol*> &, const std::unordered_set<TokenType>&);
    [[nodiscard]] Symbol* symbol(Grammar *);
    [[nodiscard]] NonTerminal* non_terminal(Grammar *);
    [[nodiscard]] Terminal* terminal(Grammar *);

    [[nodiscard]] std::string getNextId();

};


#endif //OBERON_LLVM_LOADER_H
