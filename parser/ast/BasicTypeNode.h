/*
 * Implementation of the AST basic type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_BASICTYPENODE_H
#define OBERON0C_BASICTYPENODE_H


#include <string>
#include "TypeNode.h"

class BasicTypeNode final : public TypeNode {

private:
    const std::string name_;

public:
    explicit BasicTypeNode(std::string name, int size) :
            TypeNode(NodeType::basic_type, { }, size), name_(std::move(name)) { };
    ~BasicTypeNode() final = default;

    void operator=(BasicTypeNode const&) = delete;

    const std::string getName() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

    static BasicTypeNode* BOOLEAN;
    static BasicTypeNode* INTEGER;
    static BasicTypeNode* STRING;
};


#endif //OBERON0C_BASICTYPENODE_H
