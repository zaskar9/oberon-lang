/*
 * Header file of the type reference used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/26/18.
 */

#ifndef OBERON0C_TYPEREFERENCENODE_H
#define OBERON0C_TYPEREFERENCENODE_H


#include "TypeNode.h"

class TypeReferenceNode final : public TypeNode {

private:
    const TypeNode *type_;

public:
    TypeReferenceNode(FilePos pos, const TypeNode *type);
    ~TypeReferenceNode() override;

    const TypeNode* dereference() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_TYPEREFERENCENODE_H
