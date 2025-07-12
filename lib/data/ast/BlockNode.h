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

using std::unique_ptr;
using std::vector;

class ProcedureNode;

class BlockNode {

public:
    BlockNode() : statements_(make_unique<StatementSequenceNode>(EMPTY_POS)) {}
    virtual ~BlockNode();

    [[nodiscard]] vector<unique_ptr<ConstantDeclarationNode>> &constants();
    void addConstant(std::unique_ptr<ConstantDeclarationNode> constant);
    [[nodiscard]] ConstantDeclarationNode* getConstant(size_t num) const;
    [[nodiscard]] size_t getConstantCount() const;

    [[nodiscard]] vector<unique_ptr<TypeDeclarationNode>> &types();
    void addTypeDeclaration(std::unique_ptr<TypeDeclarationNode> type_declaration);
    [[nodiscard]] TypeDeclarationNode* getTypeDeclaration(size_t num) const;
    [[nodiscard]] size_t getTypeDeclarationCount() const;

    [[nodiscard]] vector<unique_ptr<VariableDeclarationNode>> &variables();
    void addVariable(std::unique_ptr<VariableDeclarationNode> variable);
    void insertVariable(size_t pos, std::unique_ptr<VariableDeclarationNode> variable);
    [[nodiscard]] VariableDeclarationNode* getVariable(size_t num) const;
    [[nodiscard]] size_t getVariableCount() const;
    void removeVariables(size_t from, size_t to);

    [[nodiscard]] vector<unique_ptr<ProcedureNode>> &procedures();
    void addProcedure(std::unique_ptr<ProcedureNode> procedure);
    [[nodiscard]] ProcedureNode* getProcedure(size_t num) const;
    [[nodiscard]] size_t getProcedureCount() const;
    [[nodiscard]] std::unique_ptr<ProcedureNode> removeProcedure(size_t num);

    StatementSequenceNode* statements() const;

private:
    vector<unique_ptr<ConstantDeclarationNode>> constants_;
    vector<unique_ptr<TypeDeclarationNode>> type_declarations_;
    vector<unique_ptr<VariableDeclarationNode>> variables_;
    vector<unique_ptr<ProcedureNode>> procedures_;
    unique_ptr<StatementSequenceNode> statements_;

};


#endif //OBERON0C_BLOCKNODE_H
