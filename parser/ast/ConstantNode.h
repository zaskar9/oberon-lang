//
// Created by Michael Grossniklaus on 2/19/18.
//

#ifndef OBERON0C_CONSTANTNODE_H
#define OBERON0C_CONSTANTNODE_H


#include "ExpressionNode.h"

class ConstantNode : public ExpressionNode{

private:
    ExpressionType exprType_;

public:
    explicit ConstantNode(NodeType nodeType, ExpressionType exprType);
    virtual ~ConstantNode();

    virtual bool isConstant() const final;
    virtual ExpressionType checkType() const final;

};


#endif //OBERON0C_CONSTANTNODE_H
