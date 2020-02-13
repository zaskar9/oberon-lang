/*
 * Header file of the AST named-value nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H


#include "Node.h"
#include "TypeNode.h"
#include "LiteralNode.h"

class DeclarationNode : public Node {

private:
    const Node *parent_;
    const std::string name_;
    TypeNode *type_;
    int level_;

public:
    explicit DeclarationNode(const NodeType nodeType, const FilePos &pos, Node *parent,
            std::string name, TypeNode *type, int level) : Node(nodeType, pos), parent_(parent),
            name_(std::move(name)), type_(type), level_(level) { };
    ~DeclarationNode() override = default;

    const Node * getParent() const;
    const std::string getName() const;
    TypeNode * getType() const;

    int getLevel() const;

    void accept(NodeVisitor& visitor) override = 0;

    void print(std::ostream &stream) const override;

};


class ConstantDeclarationNode final : public DeclarationNode {

private:
    std::unique_ptr<LiteralNode> value_;

public:
    explicit ConstantDeclarationNode(const FilePos &pos, Node *parent, const std::string &name,
            std::unique_ptr<LiteralNode> value, int level): DeclarationNode(NodeType::constant, pos,
            parent, name, value->getType(), level), value_(std::move(value)) { };
    ~ConstantDeclarationNode() final = default;

    LiteralNode* getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class TypeDeclarationNode final : public DeclarationNode {

public:
    explicit TypeDeclarationNode(const FilePos &pos, Node *parent, const std::string &name,
            TypeNode *type, int level) : DeclarationNode(NodeType::type_declaration, pos,
            parent, name, type, level) { };
    ~TypeDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const final;

};


class VariableDeclarationNode final : public DeclarationNode {

private:
    int offset_;

public:
    explicit VariableDeclarationNode(const FilePos &pos, Node *parent, const std::string &name,
            TypeNode *type, int level, int offset) : DeclarationNode(NodeType::variable, pos,
            parent, name, type, level), offset_(offset) { };
    ~VariableDeclarationNode() final = default;

    int getOffset() const;

    void accept(NodeVisitor& visitor) override;

};


class FieldNode final : public DeclarationNode {

private:
    int offset_;

public:
    explicit FieldNode(const FilePos &pos, Node *parent, const std::string &name, TypeNode *type, int offset) :
            DeclarationNode(NodeType::field, pos, parent, name, type, -1), offset_(offset) { };
    ~FieldNode() final = default;

    int getOffset() const;

    void accept(NodeVisitor& visitor) override;

};


class ParameterNode final : public DeclarationNode {

private:
    bool var_;

public:
    explicit ParameterNode(const FilePos &pos, Node *parent, const std::string &name,
            TypeNode *type, bool var, int level) :
            DeclarationNode(NodeType::parameter, pos, parent, name, type, level), var_(var) { };
    ~ParameterNode() final = default;

    bool isVar() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_DECLARATIONNODE_H
