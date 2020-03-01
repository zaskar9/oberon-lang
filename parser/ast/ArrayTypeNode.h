/*
 * AST node representing an array type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_ARRAYTYPESYMBOL_H
#define OBERON0C_ARRAYTYPESYMBOL_H


#include <memory>
#include <utility>
#include "TypeNode.h"

class ArrayTypeNode final : public TypeNode {

private:
    const unsigned int dim_;
    TypeNode* memberType_;

public:
    explicit ArrayTypeNode(const FilePos &pos, std::string name, int dim, TypeNode* memberType) :
            TypeNode(NodeType::array_type, pos, std::move(name), dim * memberType->getSize()),
            dim_(dim), memberType_(memberType) { };
    ~ArrayTypeNode() final = default;

    [[nodiscard]] unsigned int getDimension() const;
    [[nodiscard]] TypeNode* getMemberType() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
