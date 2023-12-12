/*
 * AST node representing a basic type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_BASICTYPENODE_H
#define OBERON0C_BASICTYPENODE_H


#include "TypeNode.h"
#include <string>

class BasicTypeNode final : public TypeNode {

private:
    std::unique_ptr<Ident> ident_;

public:
    explicit BasicTypeNode(std::unique_ptr<Ident> ident, TypeKind kind, unsigned int size) :
            TypeNode(NodeType::basic_type, EMPTY_POS, ident.get(), kind, size, (int) kind), ident_(std::move(ident)) { };
    ~BasicTypeNode() final = default;

    void operator=(BasicTypeNode const&) = delete;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_BASICTYPENODE_H
