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
#include "StatementSequenceNode.h"

using std::make_unique;
using std::unique_ptr;

class LoopNode : public StatementNode {

private:
    unique_ptr<StatementSequenceNode> statements_;

protected:
    LoopNode(NodeType nodeType, const FilePos &pos, unique_ptr<StatementSequenceNode> stmts) :
            StatementNode(nodeType, pos), statements_(std::move(stmts)) { };

public:
    LoopNode(const FilePos &pos, unique_ptr<StatementSequenceNode> stmts) :
            LoopNode(NodeType::loop, pos, std::move(stmts)) { };
    ~LoopNode() override = default;

    [[nodiscard]] StatementSequenceNode * getStatements() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};

class ConditionalLoopNode : public LoopNode {

private:
    std::unique_ptr<ExpressionNode> condition_;

protected:
    ConditionalLoopNode(NodeType nodeType, const FilePos &pos, unique_ptr<ExpressionNode> condition,
                        unique_ptr<StatementSequenceNode> stmts) :
            LoopNode(nodeType, pos, std::move(stmts)), condition_(std::move(condition)) { };

public:
    ~ConditionalLoopNode() override;

    [[nodiscard]] ExpressionNode * getCondition() const;

};

class WhileLoopNode final: public ConditionalLoopNode {

public:
    WhileLoopNode(const FilePos &pos, unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts) :
            ConditionalLoopNode(NodeType::while_loop, pos, std::move(condition), std::move(stmts)) { };
    ~WhileLoopNode() override = default;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class RepeatLoopNode final : public ConditionalLoopNode {

public:
    RepeatLoopNode(const FilePos &pos, unique_ptr<ExpressionNode> condition, unique_ptr<StatementSequenceNode> stmts) :
            ConditionalLoopNode(NodeType::repeat_loop, pos, std::move(condition), std::move(stmts)) { };
    ~RepeatLoopNode() override = default;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class ForLoopNode final : public LoopNode {

private:
    unique_ptr<ValueReferenceNode> counter_;
    unique_ptr<ExpressionNode> low_, high_, step_;

public:
    ForLoopNode(const FilePos &pos, unique_ptr<ValueReferenceNode> counter,
                unique_ptr<ExpressionNode> low, unique_ptr<ExpressionNode> high, unique_ptr<ExpressionNode> step,
                unique_ptr<StatementSequenceNode> stmts) :
            LoopNode(NodeType::for_loop, pos, std::move(stmts)), counter_(std::move(counter)), low_(std::move(low)),
            high_(std::move(high)), step_(step != nullptr ? std::move(step) : make_unique<IntegerLiteralNode>(pos, 1)) { };
    ~ForLoopNode() override = default;

    [[nodiscard]] ValueReferenceNode * getCounter() const;

    void setLow(std::unique_ptr<ExpressionNode> low);
    [[nodiscard]] ExpressionNode * getLow() const;

    void setHigh(std::unique_ptr<ExpressionNode> high);
    [[nodiscard]] ExpressionNode * getHigh() const;

    void setStep(std::unique_ptr<ExpressionNode> step);
    [[nodiscard]] ExpressionNode * getStep() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_LOOPNODE_H
