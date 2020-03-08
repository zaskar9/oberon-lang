/*
 * AST node representing a module in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H


#include <memory>
#include <string>
#include "BlockNode.h"
#include "ProcedureNode.h"

class ModuleNode final : public DeclarationNode, public BlockNode {

private:
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;

public:
    explicit ModuleNode(const FilePos &pos, std::string name) :
            DeclarationNode(NodeType::module, pos, std::move(name), nullptr),
            BlockNode(pos), procedures_() { };
    ~ModuleNode() override = default;

    [[nodiscard]] NodeType getNodeType() const override {
        return DeclarationNode::getNodeType();
    }

    void addProcedure(std::unique_ptr<ProcedureNode> procedure) override;
    [[nodiscard]] ProcedureNode* getProcedure(size_t num) const override;
    [[nodiscard]] size_t getProcedureCount() const override;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_MODULENODE_H
