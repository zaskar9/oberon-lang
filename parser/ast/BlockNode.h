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
#include "StatementSequenceNode.h"
#include "ParameterNode.h"

class ProcedureNode;

class BlockNode : public Node {

private:
    int level_, offset_;

    std::vector<std::unique_ptr<TypeNode>> types_;

    std::vector<std::unique_ptr<ConstantNode>> constants_;
    std::vector<std::unique_ptr<TypeDeclarationNode>> type_declarations_;
    std::vector<std::unique_ptr<VariableNode>> variables_;
    std::unique_ptr<StatementSequenceNode> statements_;

public:
    explicit BlockNode(NodeType nodeType, FilePos pos, int level);
    ~BlockNode() override;

    int getLevel() const;
    int getOffset() const;
    void incOffset(int offset);

    void addType(std::unique_ptr<TypeNode> type);

    void addConstant(std::unique_ptr<ConstantNode> constant);
    ConstantNode* getConstant(size_t num) const;
    size_t getConstantCount() const;

    void addTypeDeclaration(std::unique_ptr<TypeDeclarationNode> type_declaration);
    TypeDeclarationNode* getTypeDeclaration(size_t num) const;
    size_t getTypeDeclarationCount() const;

    void addVariable(std::unique_ptr<VariableNode> variable);
    VariableNode* getVariable(size_t num) const;
    size_t getVariableCount() const;

    virtual void addProcedure(std::unique_ptr<ProcedureNode> procedure) = 0;
    virtual ProcedureNode* getProcedure(size_t num) const = 0;
    virtual size_t getProcedureCount() const = 0;

    StatementSequenceNode* getStatements();

    void print(std::ostream &stream) const override = 0;

};


#endif //OBERON0C_BLOCKNODE_H
