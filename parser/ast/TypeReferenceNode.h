/*
 * Header file of the AST type reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#ifndef OBERON0C_TYPEREFERENCENODE_H
#define OBERON0C_TYPEREFERENCENODE_H


#include "TypeNode.h"

class TypeReferenceNode final : public TypeNode {

private:
    const TypeNode* type_;

public:
    explicit TypeReferenceNode(FilePos pos, const TypeNode* type);
    ~TypeReferenceNode() final;

    const TypeNode* dereference() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_TYPEREFERENCENODE_H
