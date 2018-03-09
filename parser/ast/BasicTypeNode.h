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

    explicit BasicTypeNode(const std::string &name, int size);

public:
    static const std::shared_ptr<const BasicTypeNode> INTEGER;
    static const std::shared_ptr<const BasicTypeNode> BOOLEAN;
    static const std::shared_ptr<const BasicTypeNode> STRING;

    BasicTypeNode(BasicTypeNode const&) = delete;
    ~BasicTypeNode() final;

    void operator=(BasicTypeNode const&) = delete;

    const std::string getName() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_BASICTYPESYMBOL_H
