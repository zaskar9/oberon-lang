/*
 * Header of the AST statement sequence node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_STATEMENTSEQUENCENODE_H
#define OBERON0C_STATEMENTSEQUENCENODE_H


#include <vector>
#include "Node.h"
#include "StatementNode.h"

class StatementSequenceNode : public Node {

private:
    std::vector<std::unique_ptr<StatementNode>> statements_;

public:
    explicit StatementSequenceNode(FilePos pos);

    void addStatement(std::unique_ptr<StatementNode> statement);

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_STATEMENTSEQUENCENODE_H
