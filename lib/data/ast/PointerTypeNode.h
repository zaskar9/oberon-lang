//
// Created by Michael Grossniklaus on 10/1/22.
//

#ifndef OBERON_LANG_POINTERTYPENODE_H
#define OBERON_LANG_POINTERTYPENODE_H


#include "TypeNode.h"

class PointerTypeNode final : public TypeNode {

public:
    PointerTypeNode(const FilePos &pos, TypeNode *base);
    ~PointerTypeNode() override = default;

    void setBase(TypeNode *base);
    [[nodiscard]] TypeNode *getBase() const;

    [[nodiscard]] bool extends(TypeNode *) const override;

    void accept(NodeVisitor &visitor) override;

    void print(std::ostream &stream) const override;

private:
    TypeNode *base_;

};


#endif //OBERON_LANG_POINTERTYPENODE_H
