/*
 * Header file of the AST type reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/27/18.
 */

#ifndef OBERON0C_TYPEREFERENCENODE_H
#define OBERON0C_TYPEREFERENCENODE_H


#include "TypeNode.h"

class TypeReferenceNode : public TypeNode {

private:
    const TypeNode* type_;

public:
    TypeReferenceNode(FilePos pos, const TypeNode* type);
    ~TypeReferenceNode() override;

    const TypeNode* getType() const;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_TYPEREFERENCENODE_H
