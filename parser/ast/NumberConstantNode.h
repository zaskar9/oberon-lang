/*
 * Header file of the AST number constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_NUMBERCONSTANTNODE_H
#define OBERON0C_NUMBERCONSTANTNODE_H


#include "ConstantNode.h"

class NumberConstantNode final : public ConstantNode {

private:
    int value_;

public:
    explicit NumberConstantNode(int value);
    ~NumberConstantNode() override;

    const int getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_NUMBERCONSTANTNODE_H
