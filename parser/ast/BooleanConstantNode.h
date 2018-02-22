/*
 * Header file of the AST Boolean constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_BOOLEANCONSTANTNODE_H
#define OBERON0C_BOOLEANCONSTANTNODE_H


#include "ConstantNode.h"

class BooleanConstantNode final : public ConstantNode {

private:
    bool value_;

public:
    BooleanConstantNode(FilePos pos, bool value);
    ~BooleanConstantNode() override;

    const bool getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_BOOLEANCONSTANTNODE_H
