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
#include "ModuleNode.h"

using std::make_unique;
using std::unique_ptr;
using std::vector;

class ProcedureNode : public BlockNode {

private:
    bool extern_;
    bool imported_;

public:
    // ctor for use in sema / parser
    ProcedureNode(const FilePos &pos, unique_ptr<IdentDef> ident) :
            BlockNode(NodeType::procedure, pos, std::move(ident), nullptr),
            extern_(false), imported_(false) {};
    // ctor for use in symbol importer
    ProcedureNode(unique_ptr<IdentDef> ident, ProcedureTypeNode *type) :
            BlockNode(NodeType::procedure, EMPTY_POS, std::move(ident), type),
            extern_(false), imported_(true) {};
    ~ProcedureNode() override = default;

    [[nodiscard]] ProcedureTypeNode *getType() const;

    void setExtern(bool value);
    [[nodiscard]] bool isExtern() const;
    [[nodiscard]] bool isImported() const;

    [[nodiscard]] virtual bool isPredefined() const {
        return false;
    }

    void accept(NodeVisitor &visitor) override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_PROCEDURENODE_H
