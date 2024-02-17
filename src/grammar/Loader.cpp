/*
 * Utility to load a grammar from a file.
 *
 * Created by Michael Grossniklaus on 3/14/20.
 */

#include <config.h>
#include "Loader.h"
#include "../scanner/IdentToken.h"

std::unique_ptr<Grammar> Loader::load() {
    auto grammar = std::make_unique<Grammar>();
    while (scanner_.peek()->type() != TokenType::eof) {
        production(grammar.get());
    }
    return grammar;
}

// production = non_terminal [ "*" ] "=" alternation .
void Loader::production(Grammar *grammar) {
    logger_.debug("production");
    auto head = non_terminal(grammar);
    if (scanner_.peek()->type() == TokenType::op_times) {
        auto token = scanner_.next();
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
        auto token = scanner_.next();
        logger_.error(token->start(), "unexpected token (production): " + to_string(token->type()) + ".");
    }
}

// symbol = non_terminal | terminal | "(" alternation ")" | "[" alternation "]" | "{" alternation "}" .
Symbol * Loader::symbol(Grammar *grammar) {
    logger_.debug("symbol");
    auto token = scanner_.peek();
    if (token->type() == TokenType::const_ident) {
        return non_terminal(grammar);
    } else if (token->type() == TokenType::string_literal) {
        return terminal(grammar);
    } else if (token->type() == TokenType::lparen) {
        scanner_.next();
        auto head = grammar->createNonTerminal(getNextId());
        alternation(grammar, head, { TokenType::rparen });
        scanner_.next();
        return head;
    } else if (token->type() == TokenType::lbrack) {
        scanner_.next();
        auto head = grammar->createNonTerminal(getNextId());
        alternation(grammar, head, { TokenType::rbrack });
        grammar->createProduction(head, { grammar->getEpsilon() });
        scanner_.next();
        return head;
    } else if (token->type() == TokenType::lbrace) {
        scanner_.next();
        auto head = grammar->createNonTerminal(getNextId());
        auto body = grammar->createNonTerminal(getNextId());
        grammar->createProduction(head, { body, head });
        grammar->createProduction(head, { grammar->getEpsilon() });
        alternation(grammar, body, { TokenType::rbrace });
        scanner_.next();
        return head;
    }
    logger_.error(token->start(), "unexpected token (symbol): " + to_string(token->type()) + ".");
    return nullptr;
}

// symbol_list = symbol { symbol } .
void Loader::symbol_list(Grammar *grammar, std::vector<Symbol*> &symbols, const std::unordered_set<TokenType>& follows) {
    logger_.debug("symbol_list");
    auto token = scanner_.peek();
    while (follows.count(token->type()) == 0) {
        symbols.push_back(symbol(grammar));
        token = scanner_.peek();
    }
}

// non_terminal = ident .
NonTerminal * Loader::non_terminal(Grammar *grammar) {
    logger_.debug("non_terminal");
    auto token = scanner_.next();
    if (token->type() == TokenType::const_ident) {
        auto ident = dynamic_cast<const IdentToken*>(token.get());
        auto nonterminal = grammar->lookupNonTerminal(ident->value());
        return (nonterminal ? nonterminal : grammar->createNonTerminal(ident->value()));
    }
    logger_.error(token->start(), "unexpected token (non-terminal): " + to_string(token->type()) + ".");
    return nullptr;
}

// terminal = string_literal .
Terminal * Loader::terminal(Grammar *grammar) {
    logger_.debug("terminal");
    auto token = scanner_.next();
    if (token->type() == TokenType::string_literal) {
        auto literal = dynamic_cast<const StringLiteralToken*>(token.get());
        auto terminal = grammar->lookupTerminal(literal->value());
        return (terminal ? terminal : grammar->createTerminal(literal->value()));
    }
    logger_.error(token->start(), "unexpected token (terminal): " + to_string(token->type()) + ".");
    return nullptr;
}

// alternation = symbol_list { "|" symbol_list } .
void Loader::alternation(Grammar *grammar, NonTerminal *head, std::unordered_set<TokenType> follows) {
    logger_.debug("alternation");
    std::unordered_set<TokenType> followsPlus;
    followsPlus.insert(follows.begin(), follows.end());
    followsPlus.insert(TokenType::pipe);
    const Token *token;
    do {
        std::vector<Symbol *> symbols;
        symbol_list(grammar, symbols, followsPlus);
        grammar->createProduction(head, symbols);
        token = scanner_.peek();
        if (token->type() == TokenType::pipe) {
            scanner_.next();
        }
    } while (follows.count(token->type()) == 0);
}

std::string Loader::getNextId() {
    auto res = std::string("%" + to_string(id_));
    id_++;
    return res;
}