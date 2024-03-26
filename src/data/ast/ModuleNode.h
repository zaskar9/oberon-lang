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

class ModuleNode final : public BlockNode {

private:
    vector<unique_ptr<ImportNode>> imports_;

public:
    // ctor for use in sema / parser
    ModuleNode(const FilePos &pos, unique_ptr<Ident> name) :
            BlockNode(NodeType::module, pos, make_unique<IdentDef>(name->start(), name->end(), name->name()), nullptr),
            imports_() {};
    // ctor for use in symbol importer
    explicit ModuleNode(unique_ptr<Ident> name) :
            BlockNode(NodeType::module, EMPTY_POS, make_unique<IdentDef>(name->start(), name->end(), name->name()), nullptr),
            imports_() {};
    ~ModuleNode() override = default;

    [[nodiscard]] vector<unique_ptr<ImportNode>> &imports();

    void accept(NodeVisitor& visitor) override;
    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_MODULENODE_H
