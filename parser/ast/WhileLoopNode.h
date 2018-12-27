/*
 * Header of the AST while loop node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_WHILELOOPNODE_H
#define OBERON0C_WHILELOOPNODE_H

#include "ExpressionNode.h"
#include "StatementNode.h"
#include "StatementSequenceNode.h"

class WhileLoopNode : public StatementNode {

private:
    std::unique_ptr<ExpressionNode> condition_;
    std::unique_ptr<StatementSequenceNode> statements_;

public:
    WhileLoopNode(FilePos pos, std::unique_ptr<ExpressionNode> condition);
    ~WhileLoopNode() override;

    ExpressionNode* getCondition();
    StatementSequenceNode* getStatements();

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_WHILELOOPNODE_H
