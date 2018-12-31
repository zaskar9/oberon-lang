/*
 * Header file of the AST constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/7/18.
 */

#ifndef OBERON0C_CONSTANTDECLARATIONNODE_H
#define OBERON0C_CONSTANTDECLARATIONNODE_H


#include <memory>
#include "NamedValueNode.h"
#include "ValueNode.h"

class ConstantNode final : public NamedValueNode {

private:
    std::unique_ptr<ValueNode> value_;

public:
    explicit ConstantNode(FilePos pos, const std::string &name, std::unique_ptr<ValueNode> value);
    ~ConstantNode() final;

    const ValueNode* getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_CONSTANTDECLARATIONNODE_H
