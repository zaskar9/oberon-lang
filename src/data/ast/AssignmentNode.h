/*
 * AST node representing an assignment in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H


#include "StatementNode.h"
#include "NodeReference.h"

class AssignmentNode final : public StatementNode {

private:
    std::unique_ptr<ValueReferenceNode> lvalue_;
    std::unique_ptr<ExpressionNode> rvalue_;

public:
    AssignmentNode(const FilePos &pos, std::unique_ptr<ValueReferenceNode> lvalue, std::unique_ptr<ExpressionNode> rvalue) :
            StatementNode(NodeType::assignment, pos), lvalue_(std::move(lvalue)), rvalue_(std::move(rvalue)) { };
    ~AssignmentNode() override = default;

    [[nodiscard]] ValueReferenceNode * getLvalue();
    void setRvalue(std::unique_ptr<ExpressionNode> rvalue);
    [[nodiscard]] ExpressionNode * getRvalue();

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_ASSIGNMENTNODE_H
