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
#include "ImportNode.h"

class ModuleNode final : public DeclarationNode, public BlockNode {

private:
    std::vector<std::unique_ptr<ImportNode>> imports_;
    std::vector<std::unique_ptr<ModuleNode>> modules_;
    std::vector<ProcedureNode*> extprocs_;
    std::string alias_;

public:
    explicit ModuleNode(const FilePos &pos, std::unique_ptr<Ident> name) :
            DeclarationNode(NodeType::module, pos, std::move(name), nullptr),
            BlockNode(pos), imports_(), modules_(), extprocs_(), alias_() {};
    ~ModuleNode() override = default;

    [[nodiscard]] NodeType getNodeType() const override {
        return DeclarationNode::getNodeType();
    }

    void addImport(std::unique_ptr<ImportNode> import);
    [[nodiscard]] ImportNode* getImport(size_t num) const;
    [[nodiscard]] size_t getImportCount() const;

    // mainly for memory management as an anchor for smart pointers
    void addExternalModule(std::unique_ptr<ModuleNode> module);

    void addExternalProcedure(ProcedureNode *proc);
    [[nodiscard]] ProcedureNode *getExternalProcedure(size_t num) const;
    [[nodiscard]] size_t getExternalProcedureCount() const;

    void setAlias(std::string alias);
    [[nodiscard]] std::string getAlias() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_MODULENODE_H
