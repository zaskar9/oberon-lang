//
// Created by Michael Grossniklaus on 10/1/22.
//

#ifndef OBERON_LANG_POINTERTYPENODE_H
#define OBERON_LANG_POINTERTYPENODE_H


#include "TypeNode.h"

class PointerTypeNode final : public TypeNode {

private:
    TypeNode *base_;

public:
    explicit PointerTypeNode(Ident *ident, TypeNode *base) :
            TypeNode(NodeType::pointer_type, EMPTY_POS, ident, TypeKind::POINTER, 8), base_(base) {};
    ~PointerTypeNode() final = default;

    void setBase(TypeNode *base);
    [[nodiscard]] TypeNode *getBase() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON_LANG_POINTERTYPENODE_H
