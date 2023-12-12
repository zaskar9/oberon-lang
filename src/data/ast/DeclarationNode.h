/*
 * AST nodes representing declarations in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H


#include "Node.h"
#include "TypeNode.h"
#include "ExpressionNode.h"
#include "Ident.h"
#include <utility>

class TypeNode;

class DeclarationNode : public Node {

private:
    std::unique_ptr<Ident> ident_;
    TypeNode *type_;
    unsigned int level_;

public:
    explicit DeclarationNode(const NodeType nodeType, const FilePos &pos, std::unique_ptr<Ident> ident, TypeNode *type) :
            Node(nodeType, pos), ident_(std::move(ident)), type_(type), level_() { };
    ~DeclarationNode() override = default;

    void setIdentifier(std::unique_ptr<Ident> name);
    [[nodiscard]] Ident * getIdentifier() const;

    void setType(TypeNode *type);
    [[nodiscard]] TypeNode * getType() const;

    void setLevel(unsigned int level);
    [[nodiscard]] unsigned int getLevel() const;

    void accept(NodeVisitor& visitor) override = 0;

    void print(std::ostream &stream) const override;

};


class ConstantDeclarationNode final : public DeclarationNode {

private:
    std::unique_ptr<ExpressionNode> value_;

public:
    explicit ConstantDeclarationNode(const FilePos &pos, std::unique_ptr<Ident> ident, std::unique_ptr<ExpressionNode> value) :
            DeclarationNode(NodeType::constant, pos, std::move(ident), value->getType()),
            value_(std::move(value)) { };
    ~ConstantDeclarationNode() final = default;

    void setValue(std::unique_ptr<ExpressionNode> value);
    [[nodiscard]] ExpressionNode * getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class TypeDeclarationNode final : public DeclarationNode {

public:
    explicit TypeDeclarationNode(const FilePos &pos, std::unique_ptr<Ident> ident, TypeNode *type) :
            DeclarationNode(NodeType::type_declaration, pos, std::move(ident), type) { };
    ~TypeDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const final;

};


class VariableDeclarationNode final : public DeclarationNode {

public:
    explicit VariableDeclarationNode(const FilePos &pos, std::unique_ptr<Ident> ident, TypeNode *type) :
            DeclarationNode(NodeType::variable, pos, std::move(ident), type) { };
    ~VariableDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

};


class FieldNode final : public DeclarationNode {

public:
    explicit FieldNode(const FilePos &pos, std::unique_ptr<Ident> ident, TypeNode *type) :
            DeclarationNode(NodeType::field, pos, std::move(ident), type) { };
    ~FieldNode() final = default;

    void accept(NodeVisitor& visitor) override;

};


class ParameterNode final : public DeclarationNode {

private:
    bool var_;

public:
    explicit ParameterNode(const FilePos &pos, std::unique_ptr<Ident> ident, TypeNode *type, bool var) :
            DeclarationNode(NodeType::parameter, pos, std::move(ident), type), var_(var) { };
    ~ParameterNode() final = default;

    [[nodiscard]] bool isVar() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_DECLARATIONNODE_H
