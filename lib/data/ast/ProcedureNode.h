/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#ifndef OBERON0C_PROCEDURENODE_H
#define OBERON0C_PROCEDURENODE_H


#include <memory>
#include <string>
#include <vector>

#include "BlockNode.h"
#include "DeclarationNode.h"
#include "ModuleNode.h"
#include "ProcedureTypeNode.h"

using std::string;
using std::unique_ptr;
using std::vector;

enum class CallingConvention { OLANG, C };

class ProcedureNode : public DeclarationNode {

public:
    ProcedureNode(const FilePos &pos, unique_ptr<IdentDef> ident, ProcedureTypeNode *type, const CallingConvention conv) :
            DeclarationNode(NodeType::procedure, pos, std::move(ident), type), conv_(conv) {}
    ~ProcedureNode() override = 0;

    [[nodiscard]] CallingConvention getConvention() const;

    [[nodiscard]] virtual bool isExternal() const;
    [[nodiscard]] virtual bool isPredefined() const;

    [[nodiscard]] ProcedureTypeNode *getType() const override;

    void print(std::ostream &stream) const override;

private:
    CallingConvention conv_;

};


class ProcedureDeclarationNode final : public ProcedureNode {

public:
    // ctor for external procedures
    ProcedureDeclarationNode(const FilePos &pos, unique_ptr<IdentDef> ident, ProcedureTypeNode *type,
                             const CallingConvention conv, const string &name) :
            ProcedureNode(pos, std::move(ident), type, conv), name_(name) {}
    // ctor for imported procedures
    ProcedureDeclarationNode(unique_ptr<IdentDef> ident, ProcedureTypeNode *type) :
            ProcedureNode(EMPTY_POS, std::move(ident), type, CallingConvention::OLANG) {}

    ~ProcedureDeclarationNode() override = default;

    [[nodiscard]] string getName() const;

    [[nodiscard]] bool isExternal() const override;

    void accept(NodeVisitor &visitor) override;

private:
    string name_;

};


class ProcedureDefinitionNode final : public ProcedureNode, public BlockNode {

public:
    ProcedureDefinitionNode(const FilePos &pos, unique_ptr<IdentDef> ident) :
            ProcedureNode(pos, std::move(ident), nullptr, CallingConvention::OLANG) {}
    ~ProcedureDefinitionNode() override = default;

    void accept(NodeVisitor &visitor) override;

};


#endif //OBERON0C_PROCEDURENODE_H
