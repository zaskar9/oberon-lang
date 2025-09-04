/*
 * AST node representing a type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H


#include <ostream>
#include "Ident.h"
#include "Node.h"

using std::ostream;

/*
 * CAUTION: Changing these values invalidates existing symbol files!
 * - setting `NOTYPE` to zero will break symbol export and import
 * - adding more types requires adaptation of `SymbolExporter::write()` and `SymbolImporter::getXRef()`
 * - renumbering the types requires increasing the symbol file version number
 */
enum class TypeKind : char {
     // Virtual Types
    ANYTYPE = 0, NOTYPE = 1, NILTYPE = 2, ENTIRE = 3, FLOATING = 4, NUMERIC = 5,
    // Structured Types
    ARRAY = 6, POINTER = 7, PROCEDURE = 8, RECORD = 9,
    // Basic Types
    SET = 10,
    BOOLEAN = 11,
    BYTE = 12,
    CHAR = 13,
    SHORTINT = 14, INTEGER = 15, LONGINT = 16,
    REAL = 17, LONGREAL = 18,
    STRING = 19,
    // Meta-Type
    TYPE = 20
};

ostream &operator<<(ostream &stream, const TypeKind &kind);

class ASTContext;
class ModuleNode;
class TypeDeclarationNode;

class TypeNode : public Node {

public:
    TypeNode(const NodeType nodeType, const FilePos &pos, const TypeKind kind, const unsigned size) :
            Node(nodeType, pos), module_(), decl_(), kind_(kind), size_(size) {}
    ~TypeNode() override = default;

    // TODO Maybe move the module information to super class `Node`?
    [[nodiscard]] ModuleNode *getModule() const;

    [[nodiscard]] virtual Ident *getIdentifier() const;
    [[nodiscard]] TypeDeclarationNode *getDeclaration() const;

    [[nodiscard]] virtual TypeKind kind() const;

    void setSize(unsigned int);
    [[nodiscard]] virtual unsigned int getSize() const;

    [[nodiscard]] bool isAnonymous() const;

    [[nodiscard]] bool isArray() const;
    [[nodiscard]] bool isRecord() const;
    [[nodiscard]] bool isPointer() const;
    [[nodiscard]] bool isProcedure() const;
    [[nodiscard]] bool isBoolean() const;
    [[nodiscard]] bool isInteger() const;
    [[nodiscard]] bool isReal() const;
    [[nodiscard]] bool isString() const;
    [[nodiscard]] bool isSet() const;
    [[nodiscard]] bool isChar() const;
    [[nodiscard]] bool isByte() const;

    [[nodiscard]] bool isBasic() const;
    [[nodiscard]] bool isNumeric() const;
    [[nodiscard]] bool isStructured() const;

    [[nodiscard]] bool isVirtual() const;

    [[nodiscard]] virtual bool extends(TypeNode *) const;

    void accept(NodeVisitor &) override = 0;

private:
    ModuleNode *module_;
    TypeDeclarationNode *decl_;
    TypeKind kind_;
    unsigned int size_;

    void setDeclaration(TypeDeclarationNode *);
    // TODO Maybe move the module information to super class `Node`?
    void setModule(ModuleNode *);

    friend class ASTContext;
    friend class TypeDeclarationNode;

};


#endif //OBERON0C_TYPENODE_H
