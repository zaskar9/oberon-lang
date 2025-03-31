/*
 * AST node representing a statement in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H


#include "Node.h"
#include "ExpressionNode.h"

class StatementNode : public Node {

public:
    explicit StatementNode(NodeType nodeType, const FilePos &pos) : Node(nodeType, pos) {}
    ~StatementNode() override;

    [[nodiscard]] virtual bool hasExit();
    [[nodiscard]] virtual bool isReturn();

    void accept(NodeVisitor &visitor) override = 0;

};

class ReturnNode final : public StatementNode {

private:
    std::unique_ptr<ExpressionNode> value_;

public:
    ReturnNode(const FilePos &pos, std::unique_ptr<ExpressionNode> value) :
            StatementNode(NodeType::ret, pos), value_(std::move(value)) {}
    ~ReturnNode() final = default;

    [[nodiscard]] ExpressionNode * getValue() const;

    [[nodiscard]] bool isReturn() final;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


class ExitNode final : public StatementNode {

public:
    ExitNode(const FilePos &pos) : StatementNode(NodeType::exit, pos) {}
    ~ExitNode() final = default;

    [[nodiscard]] bool hasExit() final;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_STATEMENTNODE_H
