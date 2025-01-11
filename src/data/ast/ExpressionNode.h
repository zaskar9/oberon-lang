/*
 * AST nodes representing unary and binary expressions in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#ifndef OBERON0C_EXPRESSIONNODE_H
#define OBERON0C_EXPRESSIONNODE_H


#include <bitset>
#include <memory>
#include <string>
#include <vector>

#include "Node.h"
#include "TypeNode.h"
#include "BasicTypeNode.h"

using std::bitset;
using std::unique_ptr;
using std::string;
using std::vector;

enum class OperatorType : char {
    EQ, NEQ, LT, GT, GEQ, LEQ, IN, IS,
    TIMES, DIVIDE, DIV, MOD, PLUS, MINUS,
    AND, OR, NOT,
    NEG
};

std::ostream &operator<<(std::ostream &stream, const OperatorType &op);
int precedence(const OperatorType &op);

class ExpressionNode : public Node {

private:
    TypeNode *type_;
    TypeNode *cast_;

public:
    ExpressionNode(const NodeType nodeType, const FilePos &pos, TypeNode *type, TypeNode *cast = nullptr) :
            Node(nodeType, pos), type_(type), cast_(cast) {};

    ~ExpressionNode() override;

    [[nodiscard]] virtual bool isConstant() const = 0;
    [[nodiscard]] virtual int getPrecedence() const = 0;
    [[nodiscard]] virtual bool isLiteral() const { return false; };

    void setType(TypeNode *type);
    [[nodiscard]] virtual TypeNode *getType() const;

    void setCast(TypeNode *cast);
    [[nodiscard]] TypeNode *getCast() const;

    void accept(NodeVisitor &visitor) override = 0;

};


class UnaryExpressionNode final : public ExpressionNode {

private:
    OperatorType op_;
    unique_ptr<ExpressionNode> expr_;

public:
    UnaryExpressionNode(const FilePos& start, OperatorType op, unique_ptr<ExpressionNode> expr, TypeNode *type) :
            ExpressionNode(NodeType::unary_expression, start, type),
            op_(op), expr_(std::move(expr)) {};
    ~UnaryExpressionNode() final = default;

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] int getPrecedence() const final;

    [[nodiscard]] OperatorType getOperator() const;
    [[nodiscard]] ExpressionNode *getExpression() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


class BinaryExpressionNode final : public ExpressionNode {

private:
    OperatorType op_;
    unique_ptr<ExpressionNode> lhs_, rhs_;

public:
    BinaryExpressionNode(const FilePos &start, OperatorType op,
                         unique_ptr<ExpressionNode> lhs, unique_ptr<ExpressionNode> rhs, TypeNode *type) :
            ExpressionNode(NodeType::binary_expression, start, type),
            op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {};
    ~BinaryExpressionNode() final = default;

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] int getPrecedence() const final;

    [[nodiscard]] OperatorType getOperator() const;
    [[nodiscard]] ExpressionNode *getLeftExpression() const;
    [[nodiscard]] ExpressionNode *getRightExpression() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


class RangeExpressionNode final : public ExpressionNode {

private:
    unique_ptr<ExpressionNode> lower_, upper_;

public:
    RangeExpressionNode(const FilePos &start, unique_ptr<ExpressionNode> lower, unique_ptr<ExpressionNode> upper,
                        TypeNode *type) :
            ExpressionNode(NodeType::range_expression, start, type),
            lower_(std::move(lower)), upper_(std::move(upper)) {};

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] int getPrecedence() const final;

    [[nodiscard]] ExpressionNode *getLower() const;
    [[nodiscard]] ExpressionNode *getUpper() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


class SetExpressionNode final : public ExpressionNode {

private:
    vector<unique_ptr<ExpressionNode>> elements_;

public:
    SetExpressionNode(const FilePos &start, vector<unique_ptr<ExpressionNode>> elements, TypeNode *type) :
            ExpressionNode(NodeType::set_expression, start, type),
            elements_(std::move(elements)) {};

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] int getPrecedence() const final;

    [[nodiscard]] const vector<unique_ptr<ExpressionNode>> &elements() const;
    [[nodiscard]] bool isEmptySet() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


class LiteralNode : public ExpressionNode {

private:
    TypeKind kind_;

public:
    LiteralNode(const NodeType nodeType, const FilePos &pos, TypeKind kind, TypeNode *type = nullptr,
                         TypeNode *cast = nullptr) :
            ExpressionNode(nodeType, pos, type, cast), kind_(kind) {};
    ~LiteralNode() override = default;

    [[nodiscard]] TypeKind kind() const;

    void accept(NodeVisitor &visitor) override = 0;

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] bool isLiteral() const final;
    [[nodiscard]] int getPrecedence() const final;

};


class BooleanLiteralNode final : public LiteralNode {

private:
    bool value_;

public:
    BooleanLiteralNode(const FilePos &pos, bool value, TypeNode *type = nullptr, TypeNode *cast = nullptr) :
            LiteralNode(NodeType::boolean, pos, TypeKind::BOOLEAN, type, cast), value_(value) {};
    ~BooleanLiteralNode() final = default;

    [[nodiscard]] bool value() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


class IntegerLiteralNode final : public LiteralNode {

private:
    int64_t value_;

public:
    IntegerLiteralNode(const FilePos &pos, int64_t value, TypeNode *type = nullptr, TypeNode *cast = nullptr) :
            LiteralNode(NodeType::integer, pos, TypeKind::INTEGER, type, cast), value_(value) {};
    ~IntegerLiteralNode() final = default;

    [[nodiscard]] bool isShort() const;
    [[nodiscard]] bool isLong() const;
    [[nodiscard]] int64_t value() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


class RealLiteralNode final : public LiteralNode {

private:
    double value_;

public:
    RealLiteralNode(const FilePos &pos, double value, TypeNode *type = nullptr, TypeNode *cast = nullptr) :
            LiteralNode(NodeType::real, pos, TypeKind::REAL, type, cast), value_(value) {};
    ~RealLiteralNode() final = default;

    [[nodiscard]] bool isLong() const;
    [[nodiscard]] double value() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


class StringLiteralNode final : public LiteralNode {

private:
    string value_;

public:
    StringLiteralNode(const FilePos &pos, string value, TypeNode *type = nullptr, TypeNode *cast = nullptr) :
            LiteralNode(NodeType::string, pos, TypeKind::STRING, type, cast), value_(std::move(value)) {};
    ~StringLiteralNode() final = default;

    [[nodiscard]] string value() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


class CharLiteralNode final : public LiteralNode {

private:
    uint8_t value_;

public:
    CharLiteralNode(const FilePos &pos, uint8_t value, TypeNode *type = nullptr, TypeNode *cast = nullptr) :
            LiteralNode(NodeType::character, pos, TypeKind::CHAR, type, cast), value_(value) {};
    ~CharLiteralNode() final = default;

    [[nodiscard]] uint8_t value() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


class NilLiteralNode final : public LiteralNode {

public:
    NilLiteralNode(const FilePos &pos, TypeNode *type = nullptr) :
            LiteralNode(NodeType::pointer, pos, TypeKind::POINTER, type, nullptr) {};

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;
};


class SetLiteralNode final : public LiteralNode {

private:
    bitset<64> value_;

public:
    SetLiteralNode(const FilePos &pos, bitset<64> value, TypeNode *type = nullptr, TypeNode *cast = nullptr) :
            LiteralNode(NodeType::set, pos, TypeKind::SET, type, cast), value_(value) {};

    [[nodiscard]] bitset<64> value() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


class RangeLiteralNode final : public LiteralNode {

private:
    bitset<64> value_;
    int64_t lower_;
    int64_t upper_;

public:
    RangeLiteralNode(const FilePos &pos, bitset<64> value, int64_t lower, int64_t upper,
                     TypeNode *type = nullptr, TypeNode *cast = nullptr) :
            LiteralNode(NodeType::range, pos, TypeKind::SET, type, cast), value_(value), lower_(lower), upper_(upper) {};

    [[nodiscard]] bitset<64> value() const;
    [[nodiscard]] int64_t lower() const;
    [[nodiscard]] int64_t upper() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_EXPRESSIONNODE_H
