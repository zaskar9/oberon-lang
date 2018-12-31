/*
 * Header file of the AST variable reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_NAMEDVALUEDECLARATIONNODE_H
#define OBERON0C_NAMEDVALUEDECLARATIONNODE_H


#include "ExpressionNode.h"
#include "NamedValueNode.h"

class NamedValueReferenceNode final : public ExpressionNode {

private:
    NamedValueNode *node_;
    std::unique_ptr<ExpressionNode> selector_;

public:
    explicit NamedValueReferenceNode(FilePos pos, NamedValueNode *node);
    explicit NamedValueReferenceNode(FilePos pos, NamedValueNode *node, std::unique_ptr<ExpressionNode> selector);
    ~NamedValueReferenceNode() final;

    NamedValueNode* dereference() const;
    ExpressionNode* getSelector() const;

    bool isConstant() const final;
    TypeNode* getType() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_NAMEDVALUEDECLARATIONNODE_H
