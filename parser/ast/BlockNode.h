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
    std::vector<std::unique_ptr<const ConstantNode>> constants_;
    std::vector<std::shared_ptr<const TypeNode>> types_;
    std::vector<std::unique_ptr<const NamedValueNode>> variables_;
    std::vector<std::unique_ptr<const ProcedureNode>> procedures_;
    std::vector<std::unique_ptr<const Node>> statements_;

public:
    explicit BlockNode(NodeType nodeType, FilePos pos);
    ~BlockNode() override;

    void addConstant(std::unique_ptr<const ConstantNode> constant);
    void addType(const std::shared_ptr<const TypeNode> &type);
    void addVariable(std::unique_ptr<const NamedValueNode> variable);
    void addProcedure(std::unique_ptr<const ProcedureNode> procedure);
    void addStatement(std::unique_ptr<const Node> statement);

    void print(std::ostream &stream) const override = 0;

};


#endif //OBERON0C_BLOCKNODE_H
