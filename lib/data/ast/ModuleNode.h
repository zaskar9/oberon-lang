/*
 * AST node representing a module in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_MODULENODE_H
#define OBERON0C_MODULENODE_H


#include <memory>
#include <string>
#include <vector>

#include "BlockNode.h"
#include "ImportNode.h"

using std::make_unique;
using std::string;
using std::unique_ptr;
using std::vector;

class ModuleNode final : public DeclarationNode, public BlockNode {

private:
    vector<unique_ptr<ImportNode>> imports_;

public:
    // ctor for use in sema / parser
    ModuleNode(const FilePos &pos, const unique_ptr<Ident> &name) :
            DeclarationNode(NodeType::module, pos, make_unique<IdentDef>(name->start(), name->end(), name->name()), nullptr) {}
    // ctor for use in symbol importer
    explicit ModuleNode(const unique_ptr<Ident> &name) :
            DeclarationNode(NodeType::module, EMPTY_POS, make_unique<IdentDef>(name->start(), name->end(), name->name()), nullptr) {}
    ~ModuleNode() override = default;

    [[nodiscard]] vector<unique_ptr<ImportNode>> &imports();

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_MODULENODE_H
