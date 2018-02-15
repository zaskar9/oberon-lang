/*
 * Header file of the AST string constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_STRINGCONSTANTNODE_H
#define OBERON0C_STRINGCONSTANTNODE_H


#include "ExpressionNode.h"

class StringConstantNode : public ExpressionNode {

private:
    std::string value_;

public:
    StringConstantNode(const std::string &value);
    ~StringConstantNode() override;

    const std::string getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_STRINGCONSTANTNODE_H
