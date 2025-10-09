/*
 * AST nodes representing declarations in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H


#include <memory>
#include <utility>

#include "Ident.h"
#include "Node.h"
#include "ExpressionNode.h"

using std::make_unique;
using std::unique_ptr;

class ModuleNode;
class TypeNode;

class DeclarationNode : public Node {

private:
    ModuleNode *module_;
    unique_ptr<IdentDef> ident_;
    TypeNode *type_;
    unsigned int seqId_;
    unsigned int scope_;

    void setScope(unsigned int);

public:
    DeclarationNode(const NodeType nodeType, const FilePos &pos, unique_ptr<IdentDef> ident, TypeNode *type, const unsigned seqId = 0) :
            Node(nodeType, pos), module_(), ident_(std::move(ident)), type_(type), seqId_(seqId), scope_() {};
    ~DeclarationNode() override = default;

    // TODO Maybe move the module information to super class `Node`?
    virtual void setModule(ModuleNode *);
    [[nodiscard]] ModuleNode *getModule() const;

    void setIdentifier(unique_ptr<IdentDef>);
    [[nodiscard]] IdentDef *getIdentifier() const;

    void setType(TypeNode *);
    [[nodiscard]] virtual TypeNode *getType() const;

    [[nodiscard]] unsigned int seqId() const;

    [[nodiscard]] unsigned int getScope() const;

    void accept(NodeVisitor &) override = 0;

    void print(std::ostream &) const override;

    friend class LambdaLifter;
    friend class Sema;
    friend class SymbolTable;
    friend class SymbolImporter;

};


class ConstantDeclarationNode final : public DeclarationNode {

private:
    unique_ptr<ExpressionNode> value_;

public:
    // ctor for use in sema / parser
    ConstantDeclarationNode(const FilePos &pos,
                            unique_ptr<IdentDef> ident, unique_ptr<ExpressionNode> value, TypeNode *type) :
            DeclarationNode(NodeType::constant, pos, std::move(ident), type, 0),
            value_(std::move(value)) {};
    // ctor for use in symbol importer
    ConstantDeclarationNode(unique_ptr<IdentDef> ident, unique_ptr<ExpressionNode> value) :
            DeclarationNode(NodeType::constant, EMPTY_POS, std::move(ident), value->getType(), 0),
            value_(std::move(value)) {};
    ~ConstantDeclarationNode() final = default;

    [[nodiscard]] ExpressionNode * getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class TypeDeclarationNode final : public DeclarationNode {

public:
    TypeDeclarationNode(const FilePos &pos, unique_ptr<IdentDef> ident, TypeNode *type);
    ~TypeDeclarationNode() override = default;

    void setModule(ModuleNode *) override;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


class VariableDeclarationNode final : public DeclarationNode {

public:
    VariableDeclarationNode(const FilePos &pos, unique_ptr<IdentDef> ident, TypeNode *type, unsigned seqId = 0) :
            DeclarationNode(NodeType::variable, pos, std::move(ident), type, seqId) {};
    ~VariableDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class RecordTypeNode;

class FieldNode final : public DeclarationNode {

private:
    RecordTypeNode *parent_;
    unsigned index_;

    void setRecordType(RecordTypeNode *);
    void setIndex(unsigned);

public:
    FieldNode(const FilePos &pos, unique_ptr<IdentDef> ident, TypeNode *type, unsigned seqId = 0) :
            DeclarationNode(NodeType::field, pos, std::move(ident), type, seqId) {};
    ~FieldNode() final = default;

    [[nodiscard]] RecordTypeNode *getRecordType() const;
    [[nodiscard]] unsigned getIndex() const;

    void accept(NodeVisitor& visitor) override;

    friend class RecordTypeNode;

};


class ParameterNode final : public DeclarationNode {

public:
    ParameterNode(const FilePos &pos, const unique_ptr<Ident> &ident, TypeNode *type, const bool var, const unsigned seqId = 0) :
            DeclarationNode(NodeType::parameter, pos, make_unique<IdentDef>(ident->start(), ident->end(), ident->name()), type, seqId), var_(var) {};
    ~ParameterNode() override = default;

    [[nodiscard]] bool isVar() const;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

private:
    bool var_;

};


#endif //OBERON0C_DECLARATIONNODE_H
