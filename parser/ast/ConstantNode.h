/*
 * Header file of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_CONSTANTNODE_H
#define OBERON0C_CONSTANTNODE_H


#include "ExpressionNode.h"

class ConstantNode : public ExpressionNode {

private:
    ExpressionType exprType_;

public:
    explicit ConstantNode(NodeType nodeType, FilePos pos, ExpressionType exprType);
    ~ConstantNode() final;

    bool isConstant() const final;
    ExpressionType checkType() const final;

};


#endif //OBERON0C_CONSTANTNODE_H
