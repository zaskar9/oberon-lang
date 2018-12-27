/*
 * Header file of the AST number constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_NUMBERCONSTANTNODE_H
#define OBERON0C_NUMBERCONSTANTNODE_H


#include "ValueNode.h"

class NumberNode final : public ValueNode {

private:
    int value_;

public:
    explicit NumberNode(FilePos pos, int value);
    ~NumberNode() final;

    int getValue() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_NUMBERCONSTANTNODE_H
