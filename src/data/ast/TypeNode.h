/*
 * AST node representing a type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H


#include "Node.h"
#include "Ident.h"
#include <utility>

enum class TypeKind : char {
    // WARNING: Changing these values invalidates existing symbol files!
    ANYTYPE = 0, NOTYPE = 1, NILTYPE = 2, ENTIRE = 3, FLOATING = 4, NUMERIC = 5,
    ARRAY = 6, POINTER = 7, PROCEDURE = 8, RECORD = 9, SET = 10,
    BOOLEAN = 11,
    BYTE = 12, CHAR = 13, INTEGER = 14, LONGINT = 15, REAL = 16, LONGREAL = 17,
    STRING = 18
};

std::ostream &operator<<(std::ostream &stream, const TypeKind &kind);

class TypeNode : public Node {

private:
    Ident *ident_;
    TypeKind kind_;
    unsigned int size_;
    const bool anon_;
    int ref_; // used for import and export

public:
    explicit TypeNode(NodeType nodeType, const FilePos &pos, Ident *ident, TypeKind kind, unsigned int size, int ref = 0) :
            Node(nodeType, pos), ident_(ident), kind_(kind), size_(size), anon_(ident == nullptr), ref_(ref) {};
    ~TypeNode() override = default;

    [[nodiscard]] Ident *getIdentifier() const;

    [[nodiscard]] virtual TypeKind kind() const;
    void setSize(unsigned int size);
    [[nodiscard]] virtual unsigned int getSize() const;

    [[nodiscard]] bool isAnonymous() const;

    [[nodiscard]] bool isArray() const;
    [[nodiscard]] bool isRecord() const;
    [[nodiscard]] bool isPointer() const;
    [[nodiscard]] bool isProcedure() const;
    [[nodiscard]] bool isBoolean() const;
    [[nodiscard]] bool isNumeric() const;
    [[nodiscard]] bool isInteger() const;
    [[nodiscard]] bool isReal() const;
    [[nodiscard]] bool isString() const;

    void setRef(int ref);
    [[nodiscard]] int getRef() const;

    void accept(NodeVisitor &visitor) override = 0;

};


#endif //OBERON0C_TYPENODE_H
