/*
 * Header of the AST procedure call node used by the Oberon-0 compiler.
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
    virtual ~CallNode() = default;

    ProcedureNode* getProcedure() const;

    void addParameter(std::unique_ptr<ExpressionNode> parameter);
    ExpressionNode* getParameter(size_t num) const;
    size_t getParameterCount() const;

};

class FunctionCallNode : public ExpressionNode, public CallNode {

public:
    explicit FunctionCallNode(const FilePos &pos, ProcedureNode *procedure) :
        ExpressionNode(NodeType::procedure_call, pos), CallNode(NodeType::procedure_call, pos, procedure) { };
    ~FunctionCallNode() override = default;

    bool isConstant() const final;
    TypeNode* getType() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class ProcedureCallNode : public StatementNode, public CallNode {

public:
    ProcedureCallNode(FilePos pos, ProcedureNode *procedure) :
        StatementNode(NodeType::procedure_call, pos), CallNode(NodeType::procedure_call, pos, procedure) { };
    ~ProcedureCallNode() override = default;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_CALLNODE_H
