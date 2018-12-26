//
// Created by Michael Grossniklaus on 2018-12-26.
//

#ifndef OBERON0C_SYMBOL_H
#define OBERON0C_SYMBOL_H


#include "../ast/Node.h"

class Symbol {

private:
    const Node *node_;
    const int level_;
    const int offset_;

public:
    Symbol(const Node *node, const int level, const int offset);
    ~Symbol() = default;

    const Node* getNode() const;
    const int getLevel() const;
    const int getOffset() const;

};


#endif //OBERON0C_SYMBOL_H
