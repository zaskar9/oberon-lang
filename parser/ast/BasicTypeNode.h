/*
 * Implementation of the AST basic type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_BASICTYPESYMBOL_H
#define OBERON0C_BASICTYPESYMBOL_H


#include <string>
#include "TypeNode.h"

class BasicTypeNode final : public TypeNode {

private:
    const std::string name_;

public:
    BasicTypeNode(const std::string &name, int size);
    ~BasicTypeNode() override;

    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_BASICTYPESYMBOL_H
