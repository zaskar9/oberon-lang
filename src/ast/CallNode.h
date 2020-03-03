/*
 * AST nodes representing procedure or function calls in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_CALLNODE_H
#define OBERON0C_CALLNODE_H


#include "StatementNode.h"
#include "ProcedureNode.h"

class CallNode : public Node {

private:
    ProcedureNode* procedure_;
    std::vector<std::unique_ptr<ExpressionNode>> parameters_;

public:
    explicit CallNode(const NodeType type, const FilePos &pos, ProcedureNode* procedure) :
            Node(type, pos), procedure_(procedure), parameters_() { };
    ~CallNode() override = default;

    [[nodiscard]] ProcedureNode* getProcedure() const;

    void addParameter(std::unique_ptr<ExpressionNode> parameter);
    [[nodiscard]] ExpressionNode* getParameter(size_t num) const;
    [[nodiscard]] size_t getParameterCount() const;

};

class FunctionCallNode final : public ExpressionNode, public CallNode {

public:
    explicit FunctionCallNode(const FilePos &pos, ProcedureNode *procedure) :
        ExpressionNode(NodeType::procedure_call, pos), CallNode(NodeType::procedure_call, pos, procedure) { };
    ~FunctionCallNode() override = default;

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] TypeNode* getType() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class ProcedureCallNode final : public StatementNode, public CallNode {

public:
    ProcedureCallNode(FilePos pos, ProcedureNode *procedure) :
        StatementNode(NodeType::procedure_call, pos), CallNode(NodeType::procedure_call, pos, procedure) { };
    ~ProcedureCallNode() override = default;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_CALLNODE_H
