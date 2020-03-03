/*
 * AST nodes representing unary and binary expressions in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H


#include "Node.h"
#include "TypeNode.h"

enum class OperatorType : char {
    EQ, NEQ, LT, GT, GEQ, LEQ,
    TIMES, DIV, MOD, PLUS, MINUS,
    AND, OR, NOT,
    NEG
};

std::ostream& operator<<(std::ostream &stream, const OperatorType &op);

class ExpressionNode : public Node {

public:
    explicit ExpressionNode(const NodeType type, const FilePos &pos) : Node(type, pos) { };
    ~ExpressionNode() override = 0;

    [[nodiscard]] virtual bool isConstant() const = 0;
    [[nodiscard]] virtual TypeNode* getType() const = 0;

    void accept(NodeVisitor& visitor) override = 0;

};


class UnaryExpressionNode final : public ExpressionNode {

private:
    OperatorType op_;
    std::unique_ptr<ExpressionNode> expr_;

public:
    UnaryExpressionNode(const FilePos &pos, OperatorType op, std::unique_ptr<ExpressionNode> expr) :
            ExpressionNode(NodeType::unary_expression, pos), op_(op), expr_(std::move(expr)) { };
    ~UnaryExpressionNode() final = default;

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] TypeNode* getType() const final;

    [[nodiscard]] OperatorType getOperator() const;
    [[nodiscard]] ExpressionNode* getExpression() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class BinaryExpressionNode final : public ExpressionNode {

private:
    OperatorType op_;
    std::unique_ptr<ExpressionNode> lhs_, rhs_;

public:
    BinaryExpressionNode(const FilePos &pos, OperatorType op,
                         std::unique_ptr<ExpressionNode> lhs, std::unique_ptr<ExpressionNode> rhs) :
            ExpressionNode(NodeType::binary_expression, pos),
            op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) { };
    ~BinaryExpressionNode() final = default;

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] TypeNode* getType() const final;

    [[nodiscard]] OperatorType getOperator() const;
    [[nodiscard]] ExpressionNode* getLeftExpression() const;
    [[nodiscard]] ExpressionNode* getRightExpression() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_EXPRESSIONNODE_H
