/*
 * Header file of the AST binary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#ifndef OBERON0C_BINARYEXPRESSIONNODE_H
#define OBERON0C_BINARYEXPRESSIONNODE_H


#include <memory>
#include "ExpressionNode.h"

class BinaryExpressionNode final : public ExpressionNode {

private:
    OperatorType op_;
    std::unique_ptr<ExpressionNode> lhs_, rhs_;

public:
    BinaryExpressionNode(FilePos pos, OperatorType op, std::unique_ptr<ExpressionNode> lhs,
                         std::unique_ptr<ExpressionNode> rhs);
    ~BinaryExpressionNode() final;

    bool isConstant() const final;
    const TypeNode* getType() const final;

    OperatorType getOperator() const;
    const ExpressionNode* getLeftExpression() const;
    const ExpressionNode* getRightExpression() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_BINARYEXPRESSIONNODE_H
