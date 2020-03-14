/*
 * Validator that computes the FIRST, FIRST+, and FOLLOW sets
 * to check whether a context-free grammar is backtrack-free.
 *
 * Created by Michael Grossniklaus on 3/11/20.
 */

#ifndef OBERON_LLVM_VALIDATOR_H
#define OBERON_LLVM_VALIDATOR_H


#include <unordered_map>
#include "Grammar.h"

typedef std::unordered_map<Symbol*, std::unordered_set<Terminal*>> first_sets;
typedef std::unordered_map<NonTerminal*, std::unordered_set<Terminal*>> follow_sets;
typedef std::unordered_map<Production*, std::unordered_set<Terminal*>> first_plus_sets;

class Validator {

private:
    Grammar *grammar_;

public:
    explicit Validator(Grammar *grammar) : grammar_(grammar) { };
    ~Validator() = default;

    [[nodiscard]] first_sets computeFirstSets() const;
    [[nodiscard]] follow_sets computeFollowSets(first_sets firstSets) const;
    [[nodiscard]] first_plus_sets computeFirstPlusSets(first_sets firstSets, follow_sets followSets) const;
    [[nodiscard]] bool checkBacktrackFree(first_plus_sets firstPlusSets, std::pair<Production*, Production*> &pair) const;

};


#endif //OBERON_LLVM_VALIDATOR_H
