/*
 * Utility to load a grammar from a file.
 *
 * Created by Michael Grossniklaus on 3/14/20.
 */

#include "Loader.h"
#include "IdentToken.h"
#include "LiteralToken.h"

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
    const auto head = non_terminal(grammar);
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

// symbol = non_terminal | terminal | "(" alternation ")" | "[" alternation "]" | "{" alternation "}" .
Symbol * Loader::symbol(Grammar *grammar) {
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
        const auto head = grammar->createNonTerminal(getNextId());
        alternation(grammar, head, { TokenType::rparen });
        scanner_.next();
        return head;
    }
    if (token->type() == TokenType::lbrack) {
        scanner_.next();
        const auto head = grammar->createNonTerminal(getNextId());
        alternation(grammar, head, { TokenType::rbrack });
        grammar->createProduction(head, { grammar->getEpsilon() });
        scanner_.next();
        return head;
    }
    if (token->type() == TokenType::lbrace) {
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
    while (!follows.contains(token->type())) {
        symbols.push_back(symbol(grammar));
        token = scanner_.peek();
    }
}

// non_terminal = ident .
NonTerminal * Loader::non_terminal(Grammar *grammar) {
    logger_.debug("non_terminal");
    const auto token = scanner_.next();
    if (token->type() == TokenType::const_ident) {
        const auto ident = dynamic_cast<const IdentToken*>(token.get());
        const auto nonterminal = grammar->lookupNonTerminal(ident->value());
        return nonterminal ? nonterminal : grammar->createNonTerminal(ident->value());
    }
    logger_.error(token->start(), "unexpected token (non-terminal): " + to_string(token->type()) + ".");
    return nullptr;
}

// terminal = string_literal .
Terminal * Loader::terminal(Grammar *grammar) {
    logger_.debug("terminal");
    const auto token = scanner_.next();
    if (token->type() != TokenType::string_literal && token->type() != TokenType::char_literal) {
        logger_.error(token->start(), "unexpected token (terminal): " + to_string(token->type()) + ".");
        return nullptr;
    }
    std::string val;
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
    } while (!follows.contains(token->type()));
}

std::string Loader::getNextId() {
    auto res = std::string("%" + to_string(id_));
    id_++;
    return res;
}