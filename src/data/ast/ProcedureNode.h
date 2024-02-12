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

class ProcedureNode : public DeclarationNode, public BlockNode {

private:
    bool extern_;

    [[nodiscard]] ProcedureTypeNode * proctype() const;

public:
    // ctor for use in sema / parser
    ProcedureNode(const FilePos &, unique_ptr<IdentDef>, bool = false);
    // ctor for use in symbol importer
    ProcedureNode(unique_ptr<IdentDef>, ProcedureTypeNode *, bool = false);
    ~ProcedureNode() override = default;

    [[nodiscard]] NodeType getNodeType() const override {
        return DeclarationNode::getNodeType();
    }

    void addFormalParameter(unique_ptr<ParameterNode> parameter);
    [[nodiscard]] ParameterNode *getFormalParameter(const std::string &name);
    [[nodiscard]] ParameterNode *getFormalParameter(size_t num) const;
    [[nodiscard]] size_t getFormalParameterCount() const;

    void setVarArgs(bool value);
    [[nodiscard]] bool hasVarArgs() const;

    void setReturnType(TypeNode *type);
    [[nodiscard]] TypeNode *getReturnType() const;

    void setExtern(bool value);
    [[nodiscard]] bool isExtern() const;

    [[nodiscard]] virtual bool isPredefined() const {
        return false;
    }

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_PROCEDURENODE_H
