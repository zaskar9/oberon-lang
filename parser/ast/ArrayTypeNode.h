/*
 * Header file of the AST array type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_ARRAYTYPESYMBOL_H
#define OBERON0C_ARRAYTYPESYMBOL_H


#include <memory>
#include "TypeNode.h"

class ArrayTypeNode final : public TypeNode {

private:
    const int dim_;
    std::shared_ptr<const TypeNode> memberType_;

public:
    explicit ArrayTypeNode(FilePos pos, int dim, const std::shared_ptr<const TypeNode> &memberType);
    ~ArrayTypeNode() final;

    int getDimension() const;
    const std::shared_ptr<const TypeNode> getMemberType() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
