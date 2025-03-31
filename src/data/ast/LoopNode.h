/*
 * AST nodes representing loop statements in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 01/14/19.
 */

#ifndef OBERON0C_LOOPNODE_H
#define OBERON0C_LOOPNODE_H


#include <memory>

#include "AssignmentNode.h"
#include "ExpressionNode.h"
#include "IfThenElseNode.h"
#include "StatementSequenceNode.h"

using std::make_unique;
using std::ostream;
using std::unique_ptr;

class LoopNode : public StatementNode {

private:
    unique_ptr<StatementSequenceNode> statements_;

protected:
    LoopNode(NodeType nodeType, const FilePos &pos, unique_ptr<StatementSequenceNode> stmts) :
            StatementNode(nodeType, pos), statements_(std::move(stmts)) {};

public:
    LoopNode(const FilePos &pos, unique_ptr<StatementSequenceNode> stmts) :
            LoopNode(NodeType::loop, pos, std::move(stmts)) { };
    ~LoopNode() override = default;

    [[nodiscard]] StatementSequenceNode * getStatements() const;

    void accept(NodeVisitor& visitor) override;

    void print(ostream &stream) const override;

};

class ConditionalLoopNode : public LoopNode {

private:
    std::unique_ptr<ExpressionNode> condition_;

protected:
    ConditionalLoopNode(NodeType nodeType, const FilePos &pos, unique_ptr<ExpressionNode> condition,
                        unique_ptr<StatementSequenceNode> stmts) :
            LoopNode(nodeType, pos, std::move(stmts)), condition_(std::move(condition)) {};

public:
    ~ConditionalLoopNode() override;

    [[nodiscard]] ExpressionNode * getCondition() const;

};

class WhileLoopNode final : public ConditionalLoopNode {

private:
    vector<unique_ptr<ElseIfNode>> elseIfs_;

public:
    WhileLoopNode(const FilePos &pos, unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts,
                  vector<unique_ptr<ElseIfNode>> elseIfs) :
            ConditionalLoopNode(NodeType::while_loop, pos,std::move(condition), std::move(stmts)),
            elseIfs_(std::move(elseIfs)) {};
    ~WhileLoopNode() final = default;

    [[nodiscard]] ElseIfNode *getElseIf(size_t) const;
    [[nodiscard]] size_t getElseIfCount() const;
    [[nodiscard]] bool hasElseIf() const;

    void accept(NodeVisitor &visitor) final;

    void print(ostream &stream) const final;

};

class RepeatLoopNode final : public ConditionalLoopNode {

public:
    RepeatLoopNode(const FilePos &pos, unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts) :
            ConditionalLoopNode(NodeType::repeat_loop, pos, std::move(condition), std::move(stmts)) {};
    ~RepeatLoopNode() final = default;

    void accept(NodeVisitor& visitor) final;

    void print(ostream &stream) const final;

};

class ForLoopNode final : public LoopNode {

private:
    unique_ptr<QualifiedExpression> counter_;
    unique_ptr<ExpressionNode> low_, high_, step_;

public:
    ForLoopNode(const FilePos &pos, unique_ptr<QualifiedExpression> counter,
                unique_ptr<ExpressionNode> low, unique_ptr<ExpressionNode> high, unique_ptr<ExpressionNode> step,
                unique_ptr<StatementSequenceNode> stmts) :
            LoopNode(NodeType::for_loop, pos, std::move(stmts)),
            counter_(std::move(counter)), low_(std::move(low)), high_(std::move(high)), step_(std::move(step)) { };
    ~ForLoopNode() final = default;

    [[nodiscard]] QualifiedExpression *getCounter() const;
    [[nodiscard]] ExpressionNode *getLow() const;
    [[nodiscard]] ExpressionNode *getHigh() const;
    [[nodiscard]] ExpressionNode *getStep() const;

    void accept(NodeVisitor& visitor) final;

    void print(ostream &stream) const final;

};


#endif //OBERON0C_LOOPNODE_H
