/*
 * Header of the AST statement node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H


#include "Node.h"

class StatementNode : public Node {

public:
    explicit StatementNode(NodeType type, FilePos pos);
    ~StatementNode() override;

    void accept(NodeVisitor& visitor) override = 0;

};

#endif //OBERON0C_STATEMENTNODE_H
