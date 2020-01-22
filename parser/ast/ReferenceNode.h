/*
 * Header file of the AST reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_REFERENCENODE_H
#define OBERON0C_REFERENCENODE_H


#include <memory>
#include <vector>
#include "ExpressionNode.h"
#include "DeclarationNode.h"

class ReferenceNode final : public ExpressionNode {

private:
    DeclarationNode *node_;
    TypeNode *type_;
    std::vector<std::unique_ptr<ExpressionNode>> selectors_;

public:
    explicit ReferenceNode(FilePos pos, DeclarationNode *node) :
            ExpressionNode(NodeType::name_reference, pos), node_(node), type_(), selectors_() { };
    ~ReferenceNode() final = default;

    DeclarationNode* dereference() const;
    void addSelector(std::unique_ptr<ExpressionNode> selector);
    ExpressionNode* getSelector(size_t num) const;
    size_t getSelectorCount() const;

    bool isConstant() const final;
    void setType(TypeNode *type);
    TypeNode* getType() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_REFERENCENODE_H
