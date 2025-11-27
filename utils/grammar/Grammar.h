/*
 * Represents a context-free grammar.
 *
 * Created by Michael Grossniklaus on 3/10/20.
 */

#ifndef OBERON_LLVM_GRAMMAR_H
#define OBERON_LLVM_GRAMMAR_H


#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "Token.h"

class Symbol {

public:
    explicit Symbol(std::string name) : name_(std::move(name)) {}
    virtual ~Symbol();

    [[nodiscard]] std::string getName() const;
    [[nodiscard]] virtual bool isSynth() const { return false; }

    [[nodiscard]] virtual std::string toString() const = 0;
    friend std::ostream& operator<<(std::ostream &stream, const Symbol &symbol);

private:
    std::string name_;

};


class NonTerminal final : public Symbol {

public:
    explicit NonTerminal(std::string name, const bool synth = false) :
            Symbol(std::move(name)), synth_(synth) {}
    ~NonTerminal() override = default;

    [[nodiscard]] bool isSynth() const override;

    [[nodiscard]] std::string toString() const override;

private:
    bool synth_;

};


class Terminal final : public Symbol {

public:
    explicit Terminal(std::string name) : Symbol(std::move(name)) {}
    ~Terminal() override = default;

    [[nodiscard]] std::string toString() const override;

};


class Production {

public:
    Production(NonTerminal *head, std::vector<Symbol*> symbols) : head_(head), symbols_(std::move(symbols)) {}
    ~Production() = default;

    [[nodiscard]] NonTerminal * getHead() const;

    [[nodiscard]] size_t getSymbolCount() const;
    [[nodiscard]] Symbol * getSymbol(size_t num) const;

    friend std::ostream& operator<<(std::ostream &stream, const Production &production);

private:
    NonTerminal *head_;
    std::vector<Symbol*> symbols_;

};


typedef std::unordered_set<std::unique_ptr<Terminal>> Terminals;
typedef std::unordered_set<std::unique_ptr<NonTerminal>> NonTerminals;
typedef std::unordered_set<std::unique_ptr<Production>> Productions;

class Grammar {

public:
    Grammar();
    ~Grammar() = default;

    [[nodiscard]] NonTerminal* getStart() const;
    void setStart(NonTerminal* start);
    [[nodiscard]] Terminal* getEpsilon() const;
    [[nodiscard]] Terminal* getEof() const;

    [[nodiscard]] Terminal* lookupTerminal(const std::string& name);
    [[nodiscard]] Terminal* createTerminal(std::string name);
    [[nodiscard]] Terminals::const_iterator terminals_begin() const;
    [[nodiscard]] Terminals::const_iterator terminals_end() const;

    [[nodiscard]] NonTerminal* lookupNonTerminal(const std::string& name);
    [[nodiscard]] NonTerminal* createNonTerminal(std::string name, bool synth = false);
    void deleteNonTerminal(NonTerminal *);
    [[nodiscard]] NonTerminals::const_iterator nonterminals_begin() const;
    [[nodiscard]] NonTerminals::const_iterator nonterminals_end() const;

    Production* createProduction(NonTerminal *lhs, std::vector<Symbol*> rhs);
    void deleteProduction(Production*);
    [[nodiscard]] Productions::const_iterator productions_begin() const;
    [[nodiscard]] Productions::const_iterator productions_end() const;

    friend std::ostream& operator<<(std::ostream &stream, const Grammar &grammar);

private:
    Terminals terminals_;
    NonTerminals nonterminals_;
    Productions productions_;
    NonTerminal *start_;
    Terminal *epsilon_, *eof_;
    std::unordered_map<std::string, Terminal*> terminalsIdx_;
    std::unordered_map<std::string, NonTerminal*> nonterminalsIdx_;

};


#endif //OBERON_LLVM_GRAMMAR_H
