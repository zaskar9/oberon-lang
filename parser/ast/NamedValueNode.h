/*
 * Header file of the AST named-value nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#ifndef OBERON0C_NAMEDVALUENODE_H
#define OBERON0C_NAMEDVALUENODE_H


#include "Node.h"
#include "TypeNode.h"

class NamedValueNode : public Node {

private:
    const std::string name_;
    TypeNode *type_;

public:
    explicit NamedValueNode(NodeType nodeType, FilePos pos, const std::string &name, TypeNode *type);
    ~NamedValueNode() override;

    const std::string getName() const;
    TypeNode* getType() const;

    void accept(NodeVisitor& visitor) override = 0;

    void print(std::ostream &stream) const override;

};

class FieldNode final : public NamedValueNode {

public:
    explicit FieldNode(const FilePos pos, const std::string &name, TypeNode *type) :
            NamedValueNode(NodeType::field, pos, name, type) { };
    ~FieldNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class VariableNode final : public NamedValueNode {

public:
    explicit VariableNode(const FilePos pos, const std::string &name, TypeNode *type) :
            NamedValueNode(NodeType::variable, pos, name, type) { };
    ~VariableNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class TypeDeclarationNode final : public NamedValueNode {

public:
    explicit TypeDeclarationNode(const FilePos pos, const std::string &name, TypeNode *type) :
            NamedValueNode(NodeType::type_declaration, pos, name, type) { };
    ~TypeDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

};


#endif //OBERON0C_NAMEDVALUENODE_H
