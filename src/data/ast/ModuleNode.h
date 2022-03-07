/*
 * AST node representing a module in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H


#include <memory>
#include <string>
#include <optional>
#include "BlockNode.h"
#include "ImportNode.h"

class ModuleNode final : public DeclarationNode, public BlockNode {

private:
    std::vector<std::unique_ptr<ImportNode>> imports_;

public:
    explicit ModuleNode(const FilePos &pos, std::unique_ptr<Identifier> name) :
            DeclarationNode(NodeType::module, pos, std::move(name), nullptr),
            BlockNode(pos), imports_() { };
    ~ModuleNode() override = default;

    [[nodiscard]] NodeType getNodeType() const override {
        return DeclarationNode::getNodeType();
    }

    void addImport(std::unique_ptr<ImportNode> import);
    [[nodiscard]] ImportNode* getImport(size_t num) const;
    [[nodiscard]] size_t getImportCount() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_MODULENODE_H
