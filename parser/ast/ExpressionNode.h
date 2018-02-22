/*
 * Header file of the AST expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H

#include "ASTNode.h"

enum class OperatorType : char {
    EQ, NEQ, LT, GT, GEQ, LEQ,
    TIMES, DIV, MOD, PLUS, MINUS,
    AND, OR, NOT,
    NEG
};

std::ostream& operator<<(std::ostream &stream, const OperatorType &op);

enum class ExpressionType : char {
    BOOLEAN, INTEGER, STRING, UNDEF
};

class ExpressionNode : public ASTNode {

public:
    explicit ExpressionNode(NodeType type, FilePos pos);
    ~ExpressionNode() override = 0;

    virtual bool isConstant() const = 0;
    virtual ExpressionType checkType() const = 0;

};

#endif //OBERON0C_EXPRESSIONNODE_H
