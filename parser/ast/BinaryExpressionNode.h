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
    std::shared_ptr<const ExpressionNode> lhs_, rhs_;

public:
    BinaryExpressionNode(OperatorType op, std::shared_ptr<const ExpressionNode> lhs,
                         std::shared_ptr<const ExpressionNode> rhs);
    ~BinaryExpressionNode() override;

    bool isConstant() const override;
    ExpressionType checkType() const override;

    const OperatorType getOperator() const;
    const std::shared_ptr<const ExpressionNode> getLeftExpression() const;
    const std::shared_ptr<const ExpressionNode> getRightExpression() const;

    void print(std::ostream &stream) const override;

};

#endif //OBERON0C_BINARYEXPRESSIONNODE_H
