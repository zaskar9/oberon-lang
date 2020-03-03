/*
 * AST node representing an assignment in the Oberon LLVM compiler.
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
    AssignmentNode(const FilePos &pos, std::unique_ptr<ReferenceNode> lvalue, std::unique_ptr<ExpressionNode> rvalue) :
            StatementNode(NodeType::assignment, pos), lvalue_(std::move(lvalue)), rvalue_(std::move(rvalue)) { };
    ~AssignmentNode() override = default;

    [[nodiscard]] ReferenceNode* getLvalue();
    [[nodiscard]] ExpressionNode* getRvalue();

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_ASSIGNMENTNODE_H
