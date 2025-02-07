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
    uint32_t dimensions_;
    vector<uint32_t> lengths_;
    vector<TypeNode *> types_;
    ArrayTypeNode *base_;

public:
    ArrayTypeNode(const FilePos &pos, uint32_t dimensions, vector<uint32_t> lengths, vector<TypeNode *> types) :
            TypeNode(NodeType::array_type, pos, TypeKind::ARRAY, 0),
            dimensions_(dimensions), lengths_(std::move(lengths)), types_(std::move(types)), base_(nullptr) {};
    ~ArrayTypeNode() final = default;

    [[nodiscard]] uint32_t dimensions() const;
    [[nodiscard]] const vector<uint32_t> &lengths() const;
    [[nodiscard]] const vector<TypeNode *> &types() const;
    [[nodiscard]] TypeNode *getMemberType() const;

    [[nodiscard]] ArrayTypeNode *getBase() const;
    void setBase(ArrayTypeNode *);

    [[nodiscard]] bool isOpen() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
