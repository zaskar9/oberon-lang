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
    ArrayTypeNode() : ArrayTypeNode(EMPTY_POS, nullptr, 0, nullptr) {};
    ArrayTypeNode(const FilePos &pos, Ident *ident, unsigned int dimension, TypeNode *memberType) :
            TypeNode(NodeType::array_type, pos, ident, TypeKind::ARRAY, 0),
            dimension_(dimension), memberType_(memberType) {};
    ~ArrayTypeNode() final = default;

    [[nodiscard]] ExpressionNode *getExpression() const;

    void setDimension(unsigned int dim);
    [[nodiscard]] unsigned int getDimension() const;

    void setMemberType(TypeNode *memberType);
    [[nodiscard]] TypeNode *getMemberType() const;

    [[nodiscard]] bool isOpen() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
