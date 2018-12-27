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

class ElseIf {

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementSequenceNode> statements_;

public:
    ElseIf(FilePos pos, std::unique_ptr<ExpressionNode> condition);

    ExpressionNode* getCondition();
    StatementSequenceNode* getStatements();

};


class IfThenElseNode : public StatementNode {

private:

    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementSequenceNode> thenStatements_;
    std::vector<std::unique_ptr<ElseIf>> elseIfStatements_;
    std::unique_ptr<StatementSequenceNode> elseStatements_;

public:
    explicit IfThenElseNode(FilePos pos, std::unique_ptr<ExpressionNode> condition);
    ~IfThenElseNode() override;

    ExpressionNode* getCondition();
    StatementSequenceNode* addThenStatements(FilePos pos);
    StatementSequenceNode* addElseIfStatements(FilePos pos, std::unique_ptr<ExpressionNode> condition);
    StatementSequenceNode* addElseStatements(FilePos pos);

    void print(std::ostream &stream) const final;

};



#endif //OBERON0C_IFTHENELSENODE_H
