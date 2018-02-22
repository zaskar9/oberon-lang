/*
 * Header file of the AST string constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_STRINGCONSTANTNODE_H
#define OBERON0C_STRINGCONSTANTNODE_H


#include "ConstantNode.h"

class StringConstantNode final : public ConstantNode {

private:
    std::string value_;

public:
    explicit StringConstantNode(FilePos pos, const std::string &value);
    ~StringConstantNode() override;

    const std::string getValue() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_STRINGCONSTANTNODE_H
