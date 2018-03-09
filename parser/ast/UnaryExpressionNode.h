/*
 * Header file of the AST unary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#ifndef OBERON0C_UNARYEXPRESSIONNODE_H
#define OBERON0C_UNARYEXPRESSIONNODE_H


#include <memory>
#include "ExpressionNode.h"

class UnaryExpressionNode final : public ExpressionNode {

private:
    OperatorType op_;
    std::unique_ptr<const ExpressionNode> expr_;

public:
    UnaryExpressionNode(FilePos pos, OperatorType op, std::unique_ptr<const ExpressionNode> expr);
    ~UnaryExpressionNode() final;

    bool isConstant() const final;
    std::shared_ptr<const TypeNode> getType() const final;

    const OperatorType getOperator() const;
    const ExpressionNode* getExpression() const;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_UNARYEXPRESSIONNODE_H
