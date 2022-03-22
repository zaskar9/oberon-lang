/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#ifndef OBERON0C_PROCEDURENODE_H
#define OBERON0C_PROCEDURENODE_H


#include <memory>
#include <vector>
#include "BlockNode.h"
#include "DeclarationNode.h"
#include "ProcedureTypeNode.h"

class ProcedureNode final : public DeclarationNode, public BlockNode {

private:
    std::unique_ptr<ProcedureTypeNode> proctype_;
    bool extern_;

    ProcedureTypeNode *proctype() const;

public:
    explicit ProcedureNode(const FilePos &pos, std::unique_ptr<Identifier> ident);
    ~ProcedureNode() override = default;

    [[nodiscard]] NodeType getNodeType() const override {
        return DeclarationNode::getNodeType();
    }

    void addFormalParameter(std::unique_ptr<ParameterNode> parameter);
    [[nodiscard]] ParameterNode *addFormalParameter(const std::string &name);
    [[nodiscard]] ParameterNode *getFormalParameter(size_t num) const;
    [[nodiscard]] size_t getFormalParameterCount() const;

    void setVarArgs(bool value);
    [[nodiscard]] bool hasVarArgs() const;

    void setReturnType(TypeNode *type);
    [[nodiscard]] TypeNode *getReturnType() const;

    void setExtern(bool value);
    [[nodiscard]] bool isExtern() const;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_PROCEDURENODE_H
