/*
 * Utility to load a grammar from a file.
 *
 * Created by Michael Grossniklaus on 3/14/20.
 */

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "Loader.h"
#include "IdentToken.h"
#include "LiteralToken.h"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::unordered_set;
using std::vector;

unique_ptr<Grammar> Loader::load() {
    auto grammar = make_unique<Grammar>();
    while (scanner_.peek()->type() != TokenType::eof) {
        production(grammar.get());
    }
    return grammar;
}

// production = non_terminal [ "*" ] "=" alternation .
void Loader::production(Grammar *grammar) {
    logger_.debug("production");
    const auto head = non_terminal(grammar);
    id_ = 0;
    head_ = head;
    if (scanner_.peek()->type() == TokenType::op_times) {
        const auto token = scanner_.next();
        if (grammar->getStart()) {
            logger_.error(token->start(), "duplicate start symbol: " + head->getName() + ".");
        } else {
            grammar->setStart(head);
        }
    }
    if (scanner_.peek()->type() == TokenType::op_eq) {
        scanner_.next();
        alternation(grammar, head, { TokenType::period });
        scanner_.next();
    } else {
        const auto token = scanner_.next();
        logger_.error(token->start(), "unexpected token (production): " + to_string(token->type()) + ".");
    }
}

// alternation = symbol_list { "|" symbol_list } .
void Loader::alternation(Grammar *grammar, NonTerminal *head, unordered_set<TokenType> follows) {
    logger_.debug("alternation");
    unordered_set<TokenType> followsPlus;
    followsPlus.insert(follows.begin(), follows.end());
    followsPlus.insert(TokenType::pipe);
    const Token *token;
    do {
        vector<Symbol *> symbols;
        symbol_list(grammar, symbols, followsPlus);
        grammar->createProduction(head, symbols);
        token = scanner_.peek();
        if (token->type() == TokenType::pipe) {
            scanner_.next();
        }
    } while (!follows.contains(token->type()));
}

// symbol_list = symbol { symbol } .
void Loader::symbol_list(Grammar *grammar, vector<Symbol*> &symbols,
                         const unordered_set<TokenType>& follows) {
    logger_.debug("symbol_list");
    auto token = scanner_.peek();
    while (!follows.contains(token->type())) {
        symbols.push_back(symbol(grammar));
        token = scanner_.peek();
    }
}

// symbol = non_terminal | terminal | "(" alternation ")" | "[" alternation "]" | "{" alternation "}" .
Symbol* Loader::symbol(Grammar *grammar) {
    logger_.debug("symbol");
    const auto token = scanner_.peek();
    if (token->type() == TokenType::const_ident) {
        return non_terminal(grammar);
    }
    if (token->type() == TokenType::string_literal || token->type() == TokenType::char_literal) {
        return terminal(grammar);
    }
    if (token->type() == TokenType::lparen) {
        scanner_.next();
        const auto lhs = grammar->createNonTerminal(getNextId());
        alternation(grammar, lhs, { TokenType::rparen });
        scanner_.next();
        return lhs;
    }
    if (token->type() == TokenType::lbrack) {
        scanner_.next();
        const auto lhs = grammar->createNonTerminal(getNextId());
        alternation(grammar, lhs, { TokenType::rbrack });
        grammar->createProduction(lhs, { grammar->getEpsilon() });
        scanner_.next();
        return lhs;
    }
    if (token->type() == TokenType::lbrace) {
        scanner_.next();
        auto lhs =  grammar->createNonTerminal(getNextId());
        auto rhs = grammar->createNonTerminal(getNextId());
        grammar->createProduction(lhs, { rhs, lhs });
        grammar->createProduction(lhs, { grammar->getEpsilon() });
        alternation(grammar, rhs, { TokenType::rbrace });
        scanner_.next();
        return lhs;
    }
    logger_.error(token->start(), "unexpected token (symbol): " + to_string(token->type()) + ".");
    return nullptr;
}

// non_terminal = ident .
NonTerminal* Loader::non_terminal(Grammar *grammar) {
    logger_.debug("non_terminal");
    const auto token = scanner_.next();
    if (token->type() == TokenType::const_ident) {
        const auto ident = dynamic_cast<const IdentToken*>(token.get());
        const auto non_terminal = grammar->lookupNonTerminal(ident->value());
        return non_terminal ? non_terminal : grammar->createNonTerminal(ident->value());
    }
    logger_.error(token->start(), "unexpected token (non-terminal): " + to_string(token->type()) + ".");
    return nullptr;
}

// terminal = string_literal .
Terminal* Loader::terminal(Grammar *grammar) {
    logger_.debug("terminal");
    const auto token = scanner_.next();
    if (token->type() != TokenType::string_literal && token->type() != TokenType::char_literal) {
        logger_.error(token->start(), "unexpected token (terminal): " + to_string(token->type()) + ".");
        return nullptr;
    }
    string val;
    if (token->type() == TokenType::string_literal) {
        const auto literal = dynamic_cast<const StringLiteralToken*>(token.get());
        val = literal->value();
    } else if (token->type() == TokenType::char_literal) {
        const auto literal = dynamic_cast<const CharLiteralToken*>(token.get());
        val.push_back(static_cast<char>(literal->value()));
    }
    const auto terminal = grammar->lookupTerminal(val);
    return terminal ? terminal : grammar->createTerminal(val);
}

string Loader::getNextId() {
    return string(head_->toString() + to_string(id_++));
}