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

class ProcedureNode final : public DeclarationNode, public BlockNode {

private:
    std::vector<std::unique_ptr<ParameterNode>> parameters_;
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;
    bool varargs_;
    bool extern_;

public:
    explicit ProcedureNode(const FilePos &pos, std::string name) :
            DeclarationNode(NodeType::procedure, pos, std::move(name), nullptr), BlockNode(pos),
            parameters_(), procedures_(), varargs_(false), extern_(false) { };
    ~ProcedureNode() override = default;

    [[nodiscard]] NodeType getNodeType() const override {
        return DeclarationNode::getNodeType();
    }

    void addParameter(std::unique_ptr<ParameterNode> parameter);
    [[nodiscard]] ParameterNode* getParameter(size_t num) const;
    [[nodiscard]] size_t getParameterCount() const;

    void addProcedure(std::unique_ptr<ProcedureNode> procedure) override;
    [[nodiscard]] ProcedureNode* getProcedure(size_t num) const override;
    [[nodiscard]] size_t getProcedureCount() const override;

    void setVarArgs(bool value);
    [[nodiscard]] bool hasVarArgs() const;

    void setReturnType(TypeNode* type);
    [[nodiscard]] TypeNode* getReturnType() const;

    void setExtern(bool value);
    [[nodiscard]] bool isExtern() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_PROCEDURENODE_H
