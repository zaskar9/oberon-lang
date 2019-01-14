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
    int level_, offset_;

public:
    explicit NamedValueNode(NodeType nodeType, FilePos pos, const std::string &name, TypeNode *type, int level, int offset);
    ~NamedValueNode() override;

    const std::string getName() const;
    TypeNode* getType() const;

    int getLevel() const;
    int getOffset() const;

    void accept(NodeVisitor& visitor) override = 0;

    void print(std::ostream &stream) const override;

};

class FieldNode final : public NamedValueNode {

public:
    explicit FieldNode(const FilePos pos, const std::string &name, TypeNode *type, int offset) :
            NamedValueNode(NodeType::field, pos, name, type, -1, offset) { };
    ~FieldNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class VariableNode final : public NamedValueNode {

public:
    explicit VariableNode(const FilePos pos, const std::string &name, TypeNode *type, int level, int offset) :
            NamedValueNode(NodeType::variable, pos, name, type, level, offset) { };
    ~VariableNode() final = default;

    void accept(NodeVisitor& visitor) override;

};

class TypeDeclarationNode final : public NamedValueNode {

public:
    explicit TypeDeclarationNode(const FilePos pos, const std::string &name, TypeNode *type, int level) :
            NamedValueNode(NodeType::type_declaration, pos, name, type, level, -1) { };
    ~TypeDeclarationNode() final = default;

    void accept(NodeVisitor& visitor) override;

};


#endif //OBERON0C_NAMEDVALUENODE_H
