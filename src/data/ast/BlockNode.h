/*
 * AST node representing a code block in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_BLOCKNODE_H
#define OBERON0C_BLOCKNODE_H


#include "DeclarationNode.h"
#include "StatementSequenceNode.h"
#include <memory>
#include <vector>

class ProcedureNode;

class BlockNode {

private:
    std::vector<std::unique_ptr<ConstantDeclarationNode>> constants_;
    std::vector<std::unique_ptr<TypeDeclarationNode>> type_declarations_;
    std::vector<std::unique_ptr<VariableDeclarationNode>> variables_;
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;

    std::unique_ptr<StatementSequenceNode> statements_;

public:
    explicit BlockNode(const FilePos &pos);
    virtual ~BlockNode();

    [[nodiscard]] virtual NodeType getNodeType() const = 0;

    void addConstant(std::unique_ptr<ConstantDeclarationNode> constant);
    [[nodiscard]] ConstantDeclarationNode* getConstant(size_t num) const;
    [[nodiscard]] size_t getConstantCount() const;

    void addTypeDeclaration(std::unique_ptr<TypeDeclarationNode> type_declaration);
    [[nodiscard]] TypeDeclarationNode* getTypeDeclaration(size_t num) const;
    [[nodiscard]] size_t getTypeDeclarationCount() const;

    void addVariable(std::unique_ptr<VariableDeclarationNode> variable);
    void insertVariable(size_t pos, std::unique_ptr<VariableDeclarationNode> variable);
    [[nodiscard]] VariableDeclarationNode* getVariable(size_t num) const;
    [[nodiscard]] size_t getVariableCount() const;
    void removeVariables(size_t from, size_t to);

    void addProcedure(std::unique_ptr<ProcedureNode> procedure);
    [[nodiscard]] ProcedureNode* getProcedure(size_t num) const;
    [[nodiscard]] size_t getProcedureCount() const;
    [[nodiscard]] std::unique_ptr<ProcedureNode> removeProcedure(size_t num);

    StatementSequenceNode* getStatements();

};


#endif //OBERON0C_BLOCKNODE_H
