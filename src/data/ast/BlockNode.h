/*
 * AST node representing a code block in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_BLOCKNODE_H
#define OBERON0C_BLOCKNODE_H


#include <memory>
#include <vector>
#include "DeclarationNode.h"
#include "StatementSequenceNode.h"

class ProcedureNode;

class BlockNode {

private:
    std::vector<std::unique_ptr<TypeNode>> types_;

    std::vector<std::unique_ptr<ConstantDeclarationNode>> constants_;
    std::vector<std::unique_ptr<TypeDeclarationNode>> type_declarations_;
    std::vector<std::unique_ptr<VariableDeclarationNode>> variables_;
    std::unique_ptr<StatementSequenceNode> statements_;

public:
    explicit BlockNode(const FilePos &pos) : types_(), constants_(), type_declarations_(), variables_(),
            statements_(std::make_unique<StatementSequenceNode>(pos)) { };
    ~BlockNode() = default;

    void registerType(std::unique_ptr<TypeNode> type);

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

    virtual void addProcedure(std::unique_ptr<ProcedureNode> procedure) = 0;
    [[nodiscard]] virtual ProcedureNode* getProcedure(size_t num) const = 0;
    [[nodiscard]] virtual size_t getProcedureCount() const = 0;

    StatementSequenceNode* getStatements();

};


#endif //OBERON0C_BLOCKNODE_H
