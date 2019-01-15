/*
 * Header of the AST loop nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 01/14/19.
 */

#ifndef OBERON0C_LOOPNODE_H
#define OBERON0C_LOOPNODE_H

#include "AssignmentNode.h"
#include "ExpressionNode.h"
#include "StatementSequenceNode.h"

class LoopNode : public StatementNode {

private:
    std::unique_ptr<StatementSequenceNode> statements_;

public:
    explicit LoopNode(FilePos pos);
    explicit LoopNode(NodeType type, FilePos pos);
    ~LoopNode() override;

    StatementSequenceNode* getStatements() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};

class ConditionalLoopNode : public LoopNode {

private:
    std::unique_ptr<ExpressionNode> condition_;

public:
    explicit ConditionalLoopNode(NodeType type, FilePos pos, std::unique_ptr<ExpressionNode> condition);
    explicit ConditionalLoopNode(NodeType type, FilePos pos);
    ~ConditionalLoopNode() override;

    void setCondition(std::unique_ptr<ExpressionNode> condition);
    ExpressionNode* getCondition() const;

};

class WhileLoopNode final : public ConditionalLoopNode {

public:
    explicit WhileLoopNode(FilePos pos, std::unique_ptr<ExpressionNode> condition);
    ~WhileLoopNode() override;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class RepeatLoopNode final : public ConditionalLoopNode {

public:
    explicit RepeatLoopNode(FilePos pos);
    ~RepeatLoopNode() override;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class ForLoopNode final : public LoopNode {

private:
    std::unique_ptr<ReferenceNode> counter_;
    std::unique_ptr<ExpressionNode> low_, high_;
    int step_;

public:
    explicit ForLoopNode(FilePos pos, std::unique_ptr<ReferenceNode> counter,
            std::unique_ptr<ExpressionNode> low, std::unique_ptr<ExpressionNode> high, int step);
    ~ForLoopNode() override;

    ReferenceNode* getCounter() const;
    ExpressionNode* getLow() const;
    ExpressionNode* getHigh() const;
    int getStep() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_LOOPNODE_H
