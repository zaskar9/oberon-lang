/*
 * Analyzes of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/3/20.
 */

#include "Analyzer.h"

Analysis::~Analysis() = default;

void Analyzer::add(std::unique_ptr<Analysis> analysis) {
    analyses_.push_back(std::move(analysis));
}

void Analyzer::run(Node *node) {
    for (auto&& analysis : analyses_) {
        analysis->run(logger_, node);
    }
}