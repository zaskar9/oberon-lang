/*
 * Analyzes of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/3/20.
 */

#ifndef OBERON_LLVM_ANALYZER_H
#define OBERON_LLVM_ANALYZER_H


#include <memory>
#include <vector>
#include "../data/ast/Node.h"

class Analysis {

public:
    explicit Analysis() = default;
    virtual ~Analysis();

    virtual void run(Logger *logger, Node *node) = 0;

};


class Analyzer {

private:
    Logger *logger_;
    std::vector<std::unique_ptr<Analysis>> analyses_;

public:
    explicit Analyzer(Logger *logger) : logger_(logger), analyses_() { };
    ~Analyzer() = default;

    void add(std::unique_ptr<Analysis> analysis);

    void run(Node *node);

};


#endif //OBERON_LLVM_ANALYZER_H
