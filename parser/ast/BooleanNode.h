/*
 * Header file of the AST Boolean constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_BOOLEANCONSTANTNODE_H
#define OBERON0C_BOOLEANCONSTANTNODE_H


#include "ValueNode.h"

class BooleanNode final : public ValueNode {

private:
    bool value_;

public:
    BooleanNode(FilePos pos, bool value);
    ~BooleanNode() final;

    // const ValueNode* clone() const final;

    const bool getValue() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_BOOLEANCONSTANTNODE_H
