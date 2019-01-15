/*
 * Header file of the AST reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_REFERENCENODE_H
#define OBERON0C_REFERENCENODE_H


#include <memory>
#include "ExpressionNode.h"
#include "DeclarationNode.h"

class ReferenceNode final : public ExpressionNode {

private:
    DeclarationNode *node_;
    std::unique_ptr<ExpressionNode> selector_;

public:
    explicit ReferenceNode(FilePos pos, DeclarationNode *node, std::unique_ptr<ExpressionNode> selector) :
            ExpressionNode(NodeType::name_reference, pos), node_(node), selector_(std::move(selector)) { };
    explicit ReferenceNode(FilePos pos, DeclarationNode *node) : ReferenceNode(pos, node, nullptr){ };
    ~ReferenceNode() final = default;

    DeclarationNode* dereference() const;
    ExpressionNode* getSelector() const;

    bool isConstant() const final;
    TypeNode* getType() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_REFERENCENODE_H
