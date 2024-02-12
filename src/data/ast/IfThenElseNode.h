/*
 * AST node representing an if-then-elsif-else statement in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_IFTHENELSENODE_H
#define OBERON0C_IFTHENELSENODE_H


#include <memory>
#include <vector>

#include "StatementNode.h"
#include "ExpressionNode.h"
#include "StatementSequenceNode.h"

using std::unique_ptr;
using std::vector;

class ElseIfNode final : public Node {

private:
    unique_ptr<ExpressionNode> condition_;
    unique_ptr<StatementSequenceNode> statements_;

public:
    ElseIfNode(const FilePos &pos, unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts) :
            Node(NodeType::else_if, pos), condition_(std::move(condition)), statements_(std::move(stmts)) { };
    ~ElseIfNode() override = default;

    [[nodiscard]] ExpressionNode* getCondition() const;
    [[nodiscard]] StatementSequenceNode* getStatements() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class IfThenElseNode final : public StatementNode {

private:

    unique_ptr<ExpressionNode> condition_;
    unique_ptr<StatementSequenceNode> thenStatements_;
    vector<unique_ptr<ElseIfNode>> elseIfs_;
    unique_ptr<StatementSequenceNode> elseStatements_;

public:
    IfThenElseNode(const FilePos &pos, unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> thenStmts,
                   vector<unique_ptr<ElseIfNode>> elseIfs, unique_ptr<StatementSequenceNode> elseStmts) :
            StatementNode(NodeType::if_then_else, pos), condition_(std::move(condition)),
            thenStatements_(std::move(thenStmts)), elseIfs_(std::move(elseIfs)), elseStatements_(std::move(elseStmts)) {};
    ~IfThenElseNode() override = default;

    [[nodiscard]] ExpressionNode* getCondition() const;
    [[nodiscard]] StatementSequenceNode* getThenStatements() const;

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
