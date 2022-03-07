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
    bool varargs_;
    bool extern_;

public:
    explicit ProcedureNode(const FilePos &pos, std::unique_ptr<Identifier> ident) :
            DeclarationNode(NodeType::procedure, pos, std::move(ident), nullptr), BlockNode(pos),
            parameters_(), varargs_(false), extern_(false) {};
    ~ProcedureNode() override = default;

    [[nodiscard]] NodeType getNodeType() const override {
        return DeclarationNode::getNodeType();
    }

    void addParameter(std::unique_ptr<ParameterNode> parameter);
    [[nodiscard]] ParameterNode *getParameter(const std::string &name);
    [[nodiscard]] ParameterNode *getParameter(size_t num) const;
    [[nodiscard]] size_t getParameterCount() const;

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
