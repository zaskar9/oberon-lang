/*
 * Header file of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_CONSTANTNODE_H
#define OBERON0C_CONSTANTNODE_H


#include "ExpressionNode.h"
#include "BasicTypeNode.h"

class ValueNode : public ExpressionNode {

private:
    BasicTypeNode *type_;

public:
    explicit ValueNode(NodeType nodeType, FilePos pos, BasicTypeNode *type);
    ~ValueNode() override;

    void accept(NodeVisitor& visitor) override = 0;

    bool isConstant() const final;
    BasicTypeNode* getType() const final;
};


#endif //OBERON0C_CONSTANTNODE_H
