/*
 * Header of the AST if-then-else node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_IFTHENELSENODE_H
#define OBERON0C_IFTHENELSENODE_H

#include <vector>
#include "StatementNode.h"
#include "ExpressionNode.h"
#include "StatementSequenceNode.h"


class ElseIfNode : public Node {

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementSequenceNode> statements_;

public:
    ElseIfNode(FilePos pos, std::unique_ptr<ExpressionNode> condition);

    ExpressionNode* getCondition() const;
    StatementSequenceNode* getStatements() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class IfThenElseNode : public StatementNode {

private:

    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementSequenceNode> thenStatements_;
    std::vector<std::unique_ptr<ElseIfNode>> elseIfs_;
    std::unique_ptr<StatementSequenceNode> elseStatements_;

public:
    explicit IfThenElseNode(FilePos pos, std::unique_ptr<ExpressionNode> condition);
    ~IfThenElseNode() override;

    ExpressionNode* getCondition() const;
    StatementSequenceNode* addThenStatements(FilePos pos);
    StatementSequenceNode* getThenStatements() const;

    StatementSequenceNode* addElseIf(FilePos pos, std::unique_ptr<ExpressionNode> condition);
    ElseIfNode* getElseIf(size_t num) const;
    size_t getElseIfCount() const;

    StatementSequenceNode* addElseStatements(FilePos pos);
    StatementSequenceNode* getElseStatements() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};



#endif //OBERON0C_IFTHENELSENODE_H
