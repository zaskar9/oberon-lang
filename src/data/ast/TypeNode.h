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

enum class TypeKind {
    BYTE = 1, BOOLEAN = 2, CHAR = 3, INTEGER = 4, REAL = 5, SET = 6,
    POINTER = 7, NILTYPE = 8, NOTYPE = 9, PROCEURE = 10,
    STRING = 11, ARRAY = 12, RECORD = 13
};

class TypeNode : public Node {

private:
    Identifier *ident_;
    TypeKind kind_;
    unsigned int size_;
    const bool anon_;
    int ref_; // used for import and export

public:
    explicit TypeNode(NodeType nodeType, const FilePos &pos, Identifier *ident, TypeKind kind, unsigned int size, int ref = 0) :
            Node(nodeType, pos), ident_(ident), kind_(kind), size_(size), anon_(ident == nullptr), ref_(ref) {};
    ~TypeNode() override = default;

    [[nodiscard]] Identifier *getIdentifier() const;

    [[nodiscard]] virtual TypeKind kind() const;
    void setSize(unsigned int size);
    [[nodiscard]] virtual unsigned int getSize() const;

    [[nodiscard]] bool isAnonymous() const;

    void setRef(int ref);
    [[nodiscard]] int getRef() const;

    void accept(NodeVisitor &visitor) override = 0;

};


#endif //OBERON0C_TYPENODE_H
