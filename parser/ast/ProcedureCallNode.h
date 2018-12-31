/*
 * Header of the AST procedure call node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_PROCEDURECALLNODE_H
#define OBERON0C_PROCEDURECALLNODE_H


#include "StatementNode.h"
#include "ProcedureNode.h"

class ProcedureCallNode : public StatementNode {

private:
    ProcedureNode* procedure_;
    std::vector<std::unique_ptr<ExpressionNode>> parameters_;

public:
    ProcedureCallNode(FilePos pos, ProcedureNode *procedure);
    ~ProcedureCallNode() override;

    ProcedureNode* getProcedure();

    void addParameter(std::unique_ptr<ExpressionNode> parameter);
    ExpressionNode* getParameter(size_t num) const;
    size_t getParameterCount() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_PROCEDURECALLNODE_H
