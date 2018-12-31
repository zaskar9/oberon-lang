/*
 * Header of the AST unary expression node used by the Oberon-0 compiler.
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
    UnaryExpressionNode(FilePos pos, OperatorType op, std::unique_ptr<ExpressionNode> expr);
    ~UnaryExpressionNode() final;

    bool isConstant() const final;
    const TypeNode* getType() const final;

    OperatorType getOperator() const;
    const ExpressionNode* getExpression() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_UNARYEXPRESSIONNODE_H
