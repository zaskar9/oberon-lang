/*
 * AST node representing an array type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_ARRAYTYPESYMBOL_H
#define OBERON0C_ARRAYTYPESYMBOL_H


#include "TypeNode.h"
#include "ExpressionNode.h"
#include <memory>
#include <utility>

class ArrayTypeNode final : public TypeNode {

private:
    unsigned int dimension_;
    TypeNode *memberType_;

public:
    ArrayTypeNode(Ident *ident, unsigned int dimension, TypeNode *memberType) :
            TypeNode(NodeType::array_type, EMPTY_POS, ident, TypeKind::ARRAY, 0),
            dimension_(dimension), memberType_(memberType) {};
    ~ArrayTypeNode() final = default;

    [[nodiscard]] unsigned int getDimension() const;
    [[nodiscard]] TypeNode *getMemberType() const;

    [[nodiscard]] bool isOpen() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
