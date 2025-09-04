/*
 * AST node representing a basic type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_BASICTYPENODE_H
#define OBERON0C_BASICTYPENODE_H


#include <memory>
#include "TypeNode.h"

using std::unique_ptr;

class BasicTypeNode final : public TypeNode {

public:
    explicit BasicTypeNode(unique_ptr<Ident> ident, const TypeKind kind, const unsigned size) :
            TypeNode(NodeType::basic_type, EMPTY_POS, kind, size), ident_(std::move(ident)) {}
    ~BasicTypeNode() override = default;

    [[nodiscard]] Ident* getIdentifier() const override;

    void operator=(BasicTypeNode const&) = delete;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

private:
    std::unique_ptr<Ident> ident_;

};


#endif //OBERON0C_BASICTYPENODE_H
