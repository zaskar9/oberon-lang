/*
 * Header of the AST assignment node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include "NamedValueReferenceNode.h"
#include "StatementNode.h"


class AssignmentNode : public StatementNode {

private:
    std::unique_ptr<NamedValueReferenceNode> lvalue_;
    std::unique_ptr<ExpressionNode> rvalue_;

public:
    AssignmentNode(FilePos pos, std::unique_ptr<NamedValueReferenceNode> lvalue, std::unique_ptr<ExpressionNode> rvalue);
    ~AssignmentNode() override;

    NamedValueReferenceNode* getLvalue();
    ExpressionNode* getRvalue();

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_ASSIGNMENTNODE_H
