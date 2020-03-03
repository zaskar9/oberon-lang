/*
 * AST node representing a code block in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_BLOCKNODE_H
#define OBERON0C_BLOCKNODE_H


#include <memory>
#include <vector>
#include "Node.h"
#include "DeclarationNode.h"
#include "StatementSequenceNode.h"

class ProcedureNode;

class BlockNode : public Node {

private:
    int level_;

    std::vector<std::unique_ptr<TypeNode>> types_;

    std::vector<std::unique_ptr<ConstantDeclarationNode>> constants_;
    std::vector<std::unique_ptr<TypeDeclarationNode>> type_declarations_;
    std::vector<std::unique_ptr<VariableDeclarationNode>> variables_;
    std::unique_ptr<StatementSequenceNode> statements_;

    TypeNode *return_;

public:
    explicit BlockNode(NodeType nodeType, const FilePos &pos, int level);
    ~BlockNode() override;

    [[nodiscard]] int getLevel() const;

    void registerType(std::unique_ptr<TypeNode> type);

    void addConstant(std::unique_ptr<ConstantDeclarationNode> constant);
    [[nodiscard]] ConstantDeclarationNode* getConstant(size_t num) const;
    [[nodiscard]] size_t getConstantCount() const;

    void addTypeDeclaration(std::unique_ptr<TypeDeclarationNode> type_declaration);
    [[nodiscard]] TypeDeclarationNode* getTypeDeclaration(size_t num) const;
    [[nodiscard]] size_t getTypeDeclarationCount() const;

    void addVariable(std::unique_ptr<VariableDeclarationNode> variable);
    [[nodiscard]] VariableDeclarationNode* getVariable(size_t num) const;
    [[nodiscard]] size_t getVariableCount() const;

    virtual void addProcedure(std::unique_ptr<ProcedureNode> procedure) = 0;
    [[nodiscard]] virtual ProcedureNode* getProcedure(size_t num) const = 0;
    [[nodiscard]] virtual size_t getProcedureCount() const = 0;

    StatementSequenceNode* getStatements();

    void setReturnType(TypeNode* type);
    [[nodiscard]] TypeNode* getReturnType() const;

    void print(std::ostream &stream) const override = 0;

};


#endif //OBERON0C_BLOCKNODE_H
