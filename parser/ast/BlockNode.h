/*
 * Header file of the AST code block nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_BLOCKNODE_H
#define OBERON0C_BLOCKNODE_H


#include <memory>
#include <vector>
#include "Node.h"
#include "ConstantNode.h"

class ProcedureNode;

class BlockNode : public Node {

private:
    std::vector<std::shared_ptr<ConstantNode>> constants_;
    std::vector<std::shared_ptr<TypeNode>> types_;
    std::vector<std::shared_ptr<NamedValueNode>> variables_;
    std::vector<std::shared_ptr<ProcedureNode>> procedures_;
    std::vector<std::shared_ptr<Node>> statements_;

public:
    explicit BlockNode(NodeType nodeType, FilePos pos);
    ~BlockNode() override;

    void addConstant(std::shared_ptr<ConstantNode> constant);
    void addType(std::shared_ptr<TypeNode> type);
    void addVariable(std::shared_ptr<NamedValueNode> variable);
    void addProcedure(std::shared_ptr<ProcedureNode> procedure);
    void addStatement(std::shared_ptr<Node> statement);

    void print(std::ostream &stream) const override = 0;

};


#endif //OBERON0C_BLOCKNODE_H
