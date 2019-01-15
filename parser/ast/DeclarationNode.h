/*
 * Header file of the AST named-value nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H


#include "Node.h"
#include "TypeNode.h"
#include "ValueNode.h"

class DeclarationNode : public Node {

private:
    const std::string name_;
    TypeNode *type_;
    int level_, offset_;

public:
    explicit DeclarationNode(const NodeType nodeType, const FilePos pos, const std::string &name, TypeNode *type, int level, int offset) :
            Node(nodeType, pos), name_(name), type_(type), level_(level), offset_(offset) { };
    ~DeclarationNode() override = default;

    const std::string getName() const;
    TypeNode* getType() const;

    int getLevel() const;
    int getOffset() const;

    void accept(NodeVisitor& visitor) override = 0;

    void print(std::ostream &stream) const override;

};

class ConstantDeclarationNode final : public DeclarationNode {

private:
    std::unique_ptr<ValueNode> value_;

public:
    explicit ConstantDeclarationNode(FilePos pos, const std::string &name, std::unique_ptr<ValueNode> value, int level):
            DeclarationNode(NodeType::constant, pos, name, value->getType(), level, -1), value_(std::move(value)) { };
    ~ConstantDeclarationNode() final = default;

    ValueNode* getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class TypeDeclarationNode final : public DeclarationNode {

public:
    explicit TypeDeclarationNode(const FilePos pos, const std::string &name, TypeNode *type, int level) :
            DeclarationNode(NodeType::type_declaration, pos, name, type, level, -1) { };
    ~TypeDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class VariableDeclarationNode final : public DeclarationNode {

public:
    explicit VariableDeclarationNode(const FilePos pos, const std::string &name, TypeNode *type, int level, int offset) :
            DeclarationNode(NodeType::variable, pos, name, type, level, offset) { };
    ~VariableDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class FieldNode final : public DeclarationNode {

public:
    explicit FieldNode(const FilePos pos, const std::string &name, TypeNode *type, int offset) :
            DeclarationNode(NodeType::field, pos, name, type, -1, offset) { };
    ~FieldNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class ParameterNode final : public DeclarationNode {

private:
    bool var_;

public:
    explicit ParameterNode(const FilePos pos, const std::string &name, TypeNode *type, bool var, int level) :
            DeclarationNode(NodeType::parameter, pos, name, type, level, -1), var_(var) { };
    ~ParameterNode() final = default;

    bool isVar() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};



#endif //OBERON0C_DECLARATIONNODE_H
