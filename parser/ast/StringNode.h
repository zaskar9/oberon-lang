/*
 * Header file of the AST string constant nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/19/18.
 */

#ifndef OBERON0C_STRINGNODE_H
#define OBERON0C_STRINGNODE_H


#include "ValueNode.h"

class StringNode final : public ValueNode {

private:
    std::string value_;

public:
    explicit StringNode(FilePos pos, const std::string &value);
    ~StringNode() final;

    const std::string getValue() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_STRINGNODE_H
