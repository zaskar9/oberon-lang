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
    std::shared_ptr<BasicTypeNode> type_;

public:
    explicit ValueNode(NodeType nodeType, FilePos pos, std::shared_ptr<BasicTypeNode> type);
    ~ValueNode() override;

    bool isConstant() const final;
    std::shared_ptr<TypeNode> getType() const final;
};


#endif //OBERON0C_CONSTANTNODE_H
