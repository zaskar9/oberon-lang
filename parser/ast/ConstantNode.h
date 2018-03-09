/*
 * Header file of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#ifndef OBERON0C_CONSTANTDECLARATIONNODE_H
#define OBERON0C_CONSTANTDECLARATIONNODE_H


#include "NamedValueNode.h"
#include "ValueNode.h"
#include "BasicTypeNode.h"

class ConstantNode final : public NamedValueNode {

private:
    std::unique_ptr<const ValueNode> value_;

public:
    explicit ConstantNode(NodeType nodeType, FilePos pos, const std::string &name,
                          const std::shared_ptr<const BasicTypeNode> &type, std::unique_ptr<const ValueNode> value);
    ~ConstantNode() final;

    const ValueNode* getValue() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_CONSTANTDECLARATIONNODE_H
