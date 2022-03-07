/*
 * AST node representing a type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H


#include <utility>
#include "Node.h"
#include "Identifier.h"

class TypeNode : public Node {

private:
    const Identifier* ident_;
    unsigned int size_;
    const bool anon_;

public:
    explicit TypeNode(NodeType nodeType, const FilePos &pos, const Identifier* ident, unsigned int size) :
            Node(nodeType, pos), ident_(ident), size_(size), anon_(ident == nullptr) { };
    ~TypeNode() override = default;

    [[nodiscard]] const Identifier * getIdentifier() const;

    void setSize(unsigned int size);
    [[nodiscard]] virtual unsigned int getSize() const;

    [[nodiscard]] bool isAnonymous() const;

    void accept(NodeVisitor& visitor) override = 0;

};


#endif //OBERON0C_TYPENODE_H
