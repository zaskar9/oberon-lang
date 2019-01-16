/*
 * Header file of the AST literal nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_LITERALNODE_H
#define OBERON0C_LITERALNODE_H


#include "ExpressionNode.h"
#include "BasicTypeNode.h"

class LiteralNode : public ExpressionNode {

private:
    BasicTypeNode *type_;

public:
    explicit LiteralNode(NodeType nodeType, const FilePos &pos, BasicTypeNode *type) :
            ExpressionNode(nodeType, pos), type_(type) { };
    ~LiteralNode() override = default;

    void accept(NodeVisitor& visitor) override = 0;

    bool isConstant() const final;
    BasicTypeNode* getType() const final;
};


class BooleanLiteralNode final : public LiteralNode {

private:
    bool value_;

public:
    explicit BooleanLiteralNode(const FilePos &pos, bool value) :
            LiteralNode(NodeType::boolean, pos, BasicTypeNode::BOOLEAN), value_(value) { };
    ~BooleanLiteralNode() final = default;

    bool getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class IntegerLiteralNode final : public LiteralNode {

private:
    int value_;

public:
    explicit IntegerLiteralNode(const FilePos &pos, int value) :
            LiteralNode(NodeType::integer, pos, BasicTypeNode::INTEGER), value_(value) { };
    ~IntegerLiteralNode() final = default;

    int getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class StringLiteralNode final : public LiteralNode {

private:
    std::string value_;

public:
    explicit StringLiteralNode(const FilePos &pos, std::string value) :
            LiteralNode(NodeType::string, pos, BasicTypeNode::STRING), value_(std::move(value)) { };
    ~StringLiteralNode() final = default;

    const std::string getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};



#endif //OBERON0C_LITERALNODE_H
