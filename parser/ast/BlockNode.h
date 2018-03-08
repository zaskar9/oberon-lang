/*
 * Header file of the AST code block nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_BLOCKNODE_H
#define OBERON0C_BLOCKNODE_H


#include "Node.h"
#include "ProcedureNode.h"
#include "ConstantNode.h"

class BlockNode : public Node {

private:
    std::vector<std::unique_ptr<ConstantNode>> constants_;
    std::vector<std::unique_ptr<TypeNode>> types_;
    std::vector<std::unique_ptr<NamedValueNode>> variables_;
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;
    std::vector<std::unique_ptr<Node>> statements_;

public:
    explicit BlockNode(FilePos pos);
    ~BlockNode() override;


    void addConstant(std::unique_ptr<const ConstantNode> constant);
    void addType(std::unique_ptr<const TypeNode> type);
    void addVariable(std::unique_ptr<const NamedValueNode> variable);
    void addProcedure(std::unique_ptr<const ProcedureNode> procedure);
    void addStatement(std::unique_ptr<const Node> statment);

};


#endif //OBERON0C_BLOCKNODE_H
