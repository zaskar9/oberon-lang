/*
 * Validator that computes the FIRST, FIRST+, and FOLLOW sets
 * to check whether a context-free grammar is backtrack-free.
 *
 * Created by Michael Grossniklaus on 3/11/20.
 */

#include <algorithm>
#include "Validator.h"

first_sets Validator::computeFirstSets() const {
    first_sets firstSets;
    for (auto &&it = grammar_->terminals_begin(); it != grammar_->terminals_end(); ++it) {
        auto terminal = (*it).get();
        auto firstSet = std::unordered_set<Terminal *>();
        firstSet.insert(terminal);
        firstSets[terminal] = firstSet;
    }
    for (auto &&it = grammar_->nonterminals_begin(); it != grammar_->nonterminals_end(); ++it) {
        auto nonTerminal = (*it).get();
        auto firstSet = std::unordered_set<Terminal *>();
        firstSets[nonTerminal] = firstSet;
    }
    bool changes = true;
    while (changes) {
        changes = false;
        for (auto &&it = grammar_->productions_begin(); it != grammar_->productions_end(); ++it) {
            auto production = (*it).get();
            auto rhs = std::unordered_set<Terminal *>();
            auto firstSet = firstSets[production->getSymbol(0)];
            rhs.insert(firstSet.begin(), firstSet.end());
            rhs.erase(grammar_->getEpsilon());
            size_t i = 1;
            size_t k = production->getSymbolCount();
            while (firstSet.count(grammar_->getEpsilon()) && i < production->getSymbolCount()) {
                firstSet = firstSets[production->getSymbol(i)];
                rhs.insert(firstSet.begin(), firstSet.end());
                rhs.erase(grammar_->getEpsilon());
                i++;
            }
            if ((i == k) && firstSet.count(grammar_->getEpsilon())) {
                rhs.insert(grammar_->getEpsilon());
            }
            firstSet = firstSets[production->getHead()];
            rhs.insert(firstSet.begin(), firstSet.end());
            if (rhs.size() != firstSet.size()) {
                changes = true;
                firstSets[production->getHead()] = rhs;
            }
        }
    }
    return firstSets;
}

follow_sets Validator::computeFollowSets(first_sets firstSets) const {
    follow_sets followSets;
    for (auto &&it = grammar_->nonterminals_begin(); it != grammar_->nonterminals_end(); ++it) {
        auto nonterminal = (*it).get();
        auto firstSet = std::unordered_set<Terminal *>();
        followSets[nonterminal] = firstSet;
    }
    followSets[grammar_->getStart()].insert(grammar_->getEof());
    bool changes = true;
    while (changes) {
        changes = false;
        for (auto &&it = grammar_->productions_begin(); it != grammar_->productions_end(); ++it) {
            auto production = (*it).get();
            auto lhs = std::unordered_set<Terminal *>();
            auto followSet = followSets[production->getHead()];
            lhs.insert(followSet.begin(), followSet.end());
            auto k = production->getSymbolCount();
            for (int i = k - 1; i >= 0; i--) {
                auto symbol = production->getSymbol((size_t) i);
                if (auto nonterminal = dynamic_cast<NonTerminal *>(symbol)) {
                    followSet = followSets[nonterminal];
                    lhs.insert(followSet.begin(), followSet.end());
                    if (lhs.size() != followSet.size()) {
                        changes = true;
                        followSets[nonterminal] = lhs;
                    }
                    auto firstSet = firstSets[nonterminal];
                    lhs.insert(firstSet.begin(), firstSet.end());
                    if (firstSet.count(grammar_->getEpsilon())) {
                        lhs.erase(grammar_->getEpsilon());
                    }
                } else {
                    lhs = std::unordered_set<Terminal *>();
                    auto firstSet = firstSets[symbol];
                    lhs.insert(firstSet.begin(), firstSet.end());
                }
            }
        }
    }
    return followSets;
}

first_plus_sets Validator::computeFirstPlusSets(first_sets firstSets, follow_sets followSets) const {
    first_plus_sets firstPlusSets;
    for (auto &&it = grammar_->productions_begin(); it != grammar_->productions_end(); ++it) {
        auto production = (*it).get();
        auto firstPlusSet = std::unordered_set<Terminal*>();
        size_t i = 0;
        size_t k = production->getSymbolCount();
        bool epsilon = true;
        while ((i < k) && epsilon) {
            auto symbol = production->getSymbol(i);
            auto firstSet = firstSets[symbol];
            firstPlusSet.insert(firstSet.begin(), firstSet.end());
            if (firstSet.count(grammar_->getEpsilon())) {
                firstPlusSet.emplace(grammar_->getEpsilon());
                epsilon = true;
            } else {
                epsilon = false;
            }
            i++;
        }
        if ((i ==k) && epsilon) {
            auto followSet = followSets[production->getHead()];
            firstPlusSet.insert(followSet.begin(), followSet.end());
            firstPlusSet.insert(grammar_->getEpsilon());
        }
        firstPlusSets[production] = firstPlusSet;
    }
    return firstPlusSets;
}

bool Validator::checkBacktrackFree(first_plus_sets firstPlusSets, std::pair<Production*, Production*> &pair) const {
    for (auto &&outer = grammar_->productions_begin(); outer != grammar_->productions_end(); ++outer) {
        for (auto &&inner = grammar_->productions_begin(); inner != grammar_->productions_end(); ++inner) {
            if (inner != outer) {
                auto p_outer = (*outer).get();
                auto p_inner = (*inner).get();
                if (p_outer->getHead() == p_inner->getHead()) {
                    auto fps_outer = firstPlusSets[p_outer];
                    auto fps_inner = firstPlusSets[p_inner];
                    auto intersect = std::vector<Symbol*>();
                    std::set_intersection(fps_outer.begin(), fps_outer.end(),
                                          fps_inner.begin(), fps_inner.end(),
                                          std::inserter(intersect, intersect.begin()));
                    if (intersect.size() > 0) {
                        pair.first = p_outer;
                        pair.second = p_inner;
                        return false;
                    }
                }
            }
        }
    }
    return true;
}
