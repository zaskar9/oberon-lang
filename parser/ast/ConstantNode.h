//
// Created by Michael Grossniklaus on 3/7/18.
//

#ifndef OBERON0C_CONSTANTDECLARATIONNODE_H
#define OBERON0C_CONSTANTDECLARATIONNODE_H


#include "NamedValueNode.h"
#include "ValueNode.h"
#include "BasicTypeNode.h"

class ConstantNode final : public NamedValueNode {

private:
    std::unique_ptr<ValueNode> value_;

public:
    explicit ConstantNode(FilePos pos, const std::string &name, const std::shared_ptr<BasicTypeNode> &type,
                          std::unique_ptr<ValueNode> value);
    ~ConstantNode() final;

    const ValueNode* getValue() const;

    void print(std::ostream &stream) const;

};


#endif //OBERON0C_CONSTANTDECLARATIONNODE_H
