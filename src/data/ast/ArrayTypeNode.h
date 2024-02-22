/*
 * AST node representing an array type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_ARRAYTYPESYMBOL_H
#define OBERON0C_ARRAYTYPESYMBOL_H


#include "TypeNode.h"
#include "ExpressionNode.h"
#include <utility>
#include <vector>

using std::vector;

class ArrayTypeNode final : public TypeNode {

private:
    unsigned dimensions_;
    vector<unsigned> lengths_;
    TypeNode *memberType_;

public:
    ArrayTypeNode(Ident *ident, unsigned dimensions, vector<unsigned> lengths, TypeNode *memberType) :
            TypeNode(NodeType::array_type, EMPTY_POS, ident, TypeKind::ARRAY, 0),
            dimensions_(dimensions), lengths_(std::move(lengths)), memberType_(memberType) {};
    ArrayTypeNode(Ident *ident, unsigned length, TypeNode *memberType) :
            ArrayTypeNode(ident, 1, { length }, memberType) {};
    ~ArrayTypeNode() final = default;

    [[nodiscard]] unsigned dimensions() const;
    [[nodiscard]] const vector<unsigned> &lengths() const;
    [[nodiscard]] TypeNode *getMemberType() const;

    [[nodiscard]] bool isOpen() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
