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
    const ProcedureNode* procedure_;

public:
    ProcedureCallNode(FilePos pos, const ProcedureNode *procedure);
    ~ProcedureCallNode() override;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_PROCEDURECALLNODE_H
