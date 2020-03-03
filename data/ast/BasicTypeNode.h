/*
 * AST node representing a basic type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_BASICTYPENODE_H
#define OBERON0C_BASICTYPENODE_H


#include <string>
#include "TypeNode.h"

class BasicTypeNode final : public TypeNode {

public:
    explicit BasicTypeNode(std::string name, int size) :
            TypeNode(NodeType::basic_type, { }, std::move(name), size) { };
    ~BasicTypeNode() final = default;

    void operator=(BasicTypeNode const&) = delete;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

    static BasicTypeNode* BOOLEAN;
    static BasicTypeNode* CHAR;
    static BasicTypeNode* BYTE;
    static BasicTypeNode* INTEGER;
    static BasicTypeNode* LONGINT;
    static BasicTypeNode* REAL;
    static BasicTypeNode* LONGREAL;
    static BasicTypeNode* STRING;
};


#endif //OBERON0C_BASICTYPENODE_H
