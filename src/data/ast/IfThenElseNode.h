/*
 * AST node representing an if-then-elsif-else statement in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_IFTHENELSENODE_H
#define OBERON0C_IFTHENELSENODE_H


#include "StatementNode.h"
#include "ExpressionNode.h"
#include "StatementSequenceNode.h"
#include <vector>

class ElseIfNode final : public Node {

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementSequenceNode> statements_;

public:
    explicit ElseIfNode(const FilePos &pos, std::unique_ptr<ExpressionNode> condition) :
            Node(NodeType::else_if, pos), condition_(std::move(condition)),
            statements_(std::make_unique<StatementSequenceNode>(pos)) { };
    ~ElseIfNode() override = default;

    [[nodiscard]] ExpressionNode* getCondition() const;
    [[nodiscard]] StatementSequenceNode* getStatements() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class IfThenElseNode final : public StatementNode {

private:

    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementSequenceNode> thenStatements_;
    std::vector<std::unique_ptr<ElseIfNode>> elseIfs_;
    std::unique_ptr<StatementSequenceNode> elseStatements_;

public:
    explicit IfThenElseNode(const FilePos &pos, std::unique_ptr<ExpressionNode> condition) :
            StatementNode(NodeType::if_then_else, pos), condition_(std::move(condition)), elseIfs_() { };
    ~IfThenElseNode() override = default;

    [[nodiscard]] ExpressionNode* getCondition() const;
    [[nodiscard]] StatementSequenceNode* addThenStatements(FilePos pos);
    [[nodiscard]] StatementSequenceNode* getThenStatements() const;

    [[nodiscard]] StatementSequenceNode* addElseIf(FilePos pos, std::unique_ptr<ExpressionNode> condition);
    [[nodiscard]] ElseIfNode* getElseIf(size_t num) const;
    [[nodiscard]] size_t getElseIfCount() const;
    [[nodiscard]] bool hasElseIf() const;

    [[nodiscard]] StatementSequenceNode* addElseStatements(FilePos pos);
    [[nodiscard]] StatementSequenceNode* getElseStatements() const;
    [[nodiscard]] bool hasElse() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_IFTHENELSENODE_H
