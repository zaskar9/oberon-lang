/*
 * AST node representing a record type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_RECORDTYPESYMBOL_H
#define OBERON0C_RECORDTYPESYMBOL_H


#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "TypeNode.h"
#include "DeclarationNode.h"

using std::ostream;
using std::string;
using std::unique_ptr;
using std::vector;


class RecordTypeNode final : public TypeNode {

private:
    vector<unique_ptr<FieldNode>> fields_;
    RecordTypeNode *base_;
    unsigned short level_;

public:
    RecordTypeNode(const FilePos &pos, Ident *ident, RecordTypeNode *base, vector<unique_ptr<FieldNode>> fields) :
            TypeNode(NodeType::record_type, pos, ident, TypeKind::RECORD, 0),
            fields_(std::move(fields)), base_(base), level_(base ? base->level() + 1 : 0) {};
    ~RecordTypeNode() final = default;

    [[nodiscard]] unsigned int getSize() const final;

    [[nodiscard]] FieldNode *getField(const string &name) const;
    [[nodiscard]] FieldNode *getField(size_t num) const;
    [[nodiscard]] size_t getFieldCount();

    [[nodiscard]] RecordTypeNode *getBaseType() const;
    [[nodiscard]] bool isExtened() const;
    [[nodiscard]] bool extends(TypeNode *) const override;

    [[nodiscard]] unsigned short level() const;

    void accept(NodeVisitor &visitor) final;
    void print(ostream &out) const final;

};


#endif //OBERON0C_RECORDTYPESYMBOL_H
