/*
 * Header file of the AST constant reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/28/18.
 */

#ifndef OBERON0C_CONSTANTREFERENCENODE_H
#define OBERON0C_CONSTANTREFERENCENODE_H


#include "ConstantNode.h"

class ConstantReferenceNode final : public ConstantNode {

private:
    const ConstantNode* constant_;

public:
    explicit ConstantReferenceNode(FilePos pos, const ConstantNode* constant);
    ~ConstantReferenceNode() final;

    const ConstantNode* dereference() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_CONSTANTREFERENCENODE_H
