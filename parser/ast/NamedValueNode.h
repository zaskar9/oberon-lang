/*
 * Header file of the AST named-value nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */


#ifndef OBERON0C_NAMEDVALUENODE_H
#define OBERON0C_NAMEDVALUENODE_H


#include "Node.h"
#include "TypeNode.h"
#include "ValueNode.h"

class NamedValueNode : public Node {

private:
    const std::string name_;
    const TypeNode *type_;

public:
    explicit NamedValueNode(NodeType nodeType, FilePos pos, const std::string &name, const TypeNode *type);
    ~NamedValueNode() override;

    const std::string getName() const;
    const TypeNode* getType() const;

    void print(std::ostream &stream) const override;

};

class FieldNode final : public NamedValueNode {

public:
    explicit FieldNode(const FilePos pos, const std::string &name, TypeNode *type) :
            NamedValueNode(NodeType::field, pos, name, type) { };
    ~FieldNode() final = default;
};

class VariableNode final : public NamedValueNode {

public:
    explicit VariableNode(const FilePos pos, const std::string &name, TypeNode *type) :
            NamedValueNode(NodeType::variable, pos, name, type) { };
    ~VariableNode() final = default;
};


#endif //OBERON0C_NAMEDVALUENODE_H
