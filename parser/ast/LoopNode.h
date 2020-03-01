/*
 * AST nodes representing loop statements in the Oberon LLVM compiler.
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
    explicit LoopNode(NodeType type, const FilePos &pos) : StatementNode(type, pos),
            statements_(std::make_unique<StatementSequenceNode>(pos)) { };
    explicit LoopNode(const FilePos &pos) : LoopNode(NodeType::loop, pos) { };
    ~LoopNode() override = default;

    [[nodiscard]] StatementSequenceNode* getStatements() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};

class ConditionalLoopNode : public LoopNode {

private:
    std::unique_ptr<ExpressionNode> condition_;

public:
    explicit ConditionalLoopNode(NodeType type, const FilePos &pos, std::unique_ptr<ExpressionNode> condition) :
            LoopNode(type, pos), condition_(std::move(condition)) { };
    explicit ConditionalLoopNode(NodeType type, const FilePos &pos) :
            ConditionalLoopNode(type, pos, nullptr) { };
    ~ConditionalLoopNode() override = default;

    void setCondition(std::unique_ptr<ExpressionNode> condition);
    [[nodiscard]] ExpressionNode* getCondition() const;

};

class WhileLoopNode final: public ConditionalLoopNode {

public:
    explicit WhileLoopNode(const FilePos &pos, std::unique_ptr<ExpressionNode> condition) :
            ConditionalLoopNode(NodeType::while_loop, pos, std::move(condition)) { };
    ~WhileLoopNode() override = default;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class RepeatLoopNode final : public ConditionalLoopNode {

public:
    explicit RepeatLoopNode(const FilePos &pos) : ConditionalLoopNode(NodeType::repeat_loop, pos) { };
    ~RepeatLoopNode() override = default;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class ForLoopNode final : public LoopNode {

private:
    std::unique_ptr<ReferenceNode> counter_;
    std::unique_ptr<ExpressionNode> low_, high_;
    int step_;

public:
    explicit ForLoopNode(const FilePos &pos, std::unique_ptr<ReferenceNode> counter,
            std::unique_ptr<ExpressionNode> low, std::unique_ptr<ExpressionNode> high, int step) :
            LoopNode(NodeType::for_loop, pos), counter_(std::move(counter)),
            low_(std::move(low)), high_(std::move(high)), step_(step) { };
    ~ForLoopNode() override = default;

    [[nodiscard]] ReferenceNode* getCounter() const;
    [[nodiscard]] ExpressionNode* getLow() const;
    [[nodiscard]] ExpressionNode* getHigh() const;
    [[nodiscard]] int getStep() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_LOOPNODE_H
