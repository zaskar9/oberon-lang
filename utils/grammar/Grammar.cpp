/*
 * Represents a context-free grammar.
 *
 * Created by Michael Grossniklaus on 3/10/20.
 */

#include "Grammar.h"

Symbol::~Symbol() = default;

std::string Symbol::getName() const {
    return name_;
}

std::ostream& operator<<(std::ostream &stream, const Symbol &symbol) {
    return stream << symbol.toString();
}

std::string Terminal::toString() const {
    std::stringstream ss;
    ss << "<" << getName() << ">";
    return ss.str();
}

std::string NonTerminal::toString() const {
    return getName();
}

NonTerminal * Production::getHead() const {
    return head_;
}

size_t Production::getSymbolCount() const {
    return symbols_.size();
}

Symbol * Production::getSymbol(const size_t num) const {
    return symbols_.at(num);
}

std::ostream& operator<<(std::ostream &stream, const Production &production) {
    std::stringstream ss;
    ss << production.head_->getName() << " →";
    for (auto&& symbol: production.symbols_) {
        ss << " " << *symbol;
    }
    return stream << ss.str();
}

Grammar::Grammar() : start_() {
    epsilon_ = createTerminal("\u03B5");
    eof_ = createTerminal("\u2205");
}

NonTerminal * Grammar::getStart() const {
    return start_;
}

void Grammar::setStart(NonTerminal *start) {
    start_ = start;
}

Terminal * Grammar::getEpsilon() const {
    return epsilon_;
}

Terminal * Grammar::getEof() const {
    return eof_;
}

Terminal * Grammar::lookupTerminal(const std::string& name) {
    return terminalsIdx_[name];
}

Terminal * Grammar::createTerminal(std::string name) {
    auto symbol = std::make_unique<Terminal>(std::move(name));
    const auto result = symbol.get();
    terminals_.insert(std::move(symbol));
    terminalsIdx_[result->getName()] = result;
    return result;
}

Terminals::const_iterator Grammar::terminals_begin() const {
    return terminals_.begin();
}

Terminals::const_iterator Grammar::terminals_end() const {
    return terminals_.end();
}

NonTerminal * Grammar::lookupNonTerminal(const std::string& name) {
    return nonterminalsIdx_[name];
}

NonTerminal * Grammar::createNonTerminal(std::string name, const bool isStart) {
    auto symbol = std::make_unique<NonTerminal>(std::move(name));
    const auto result = symbol.get();
    if (isStart) {
        start_ = result;
    }
    nonterminals_.insert(std::move(symbol));
    nonterminalsIdx_[result->getName()] = result;
    return result;
}

NonTerminals::const_iterator Grammar::nonterminals_begin() const {
    return nonterminals_.begin();
}

NonTerminals::const_iterator Grammar::nonterminals_end() const {
    return nonterminals_.end();
}

Production * Grammar::createProduction(NonTerminal *lhs, std::vector<Symbol *> rhs) {
    auto production = std::make_unique<Production>(lhs, std::move(rhs));
    const auto result = production.get();
    productions_.insert(std::move(production));
    return result;
}

Productions::const_iterator Grammar::productions_begin() const {
    return productions_.begin();
}

Productions::const_iterator Grammar::productions_end() const {
    return productions_.end();
}

std::ostream& operator<<(std::ostream &stream, const Grammar &grammar) {
    std::stringstream ss;
    for (auto &&it = grammar.productions_begin(); it != grammar.productions_end(); ++it) {
        const auto production = it->get();
        ss << *production << std::endl;
    }
    return stream << ss.str();
}

