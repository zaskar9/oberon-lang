/*
 * Header of the AST assignment node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H


#include "StatementNode.h"
#include "ReferenceNode.h"

class AssignmentNode final : public StatementNode {

private:
    std::unique_ptr<ReferenceNode> lvalue_;
    std::unique_ptr<ExpressionNode> rvalue_;

public:
    AssignmentNode(FilePos pos, std::unique_ptr<ReferenceNode> lvalue, std::unique_ptr<ExpressionNode> rvalue);
    ~AssignmentNode() override;

    ReferenceNode* getLvalue();
    ExpressionNode* getRvalue();

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_ASSIGNMENTNODE_H
