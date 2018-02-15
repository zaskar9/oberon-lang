/*
 * Header file of the AST unary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#ifndef OBERON0C_UNARYEXPRESSIONNODE_H
#define OBERON0C_UNARYEXPRESSIONNODE_H


#include <memory>
#include "ExpressionNode.h"

class UnaryExpressionNode : public ExpressionNode {

private:
    OperatorType op_;
    std::shared_ptr<const ExpressionNode> expr_;

public:
    UnaryExpressionNode(OperatorType op, std::shared_ptr<const ExpressionNode> expr);
    ~UnaryExpressionNode() override;

    const OperatorType getOperator() const;
    const std::shared_ptr<const ExpressionNode> getExpression() const;

    void print(std::ostream &stream) const override;

};

#endif //OBERON0C_UNARYEXPRESSIONNODE_H
