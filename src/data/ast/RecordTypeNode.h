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

public:
    RecordTypeNode(const FilePos &pos, RecordTypeNode *base, vector<unique_ptr<FieldNode>> fields) :
            TypeNode(NodeType::record_type, pos, TypeKind::RECORD, 0),
            fields_(std::move(fields)), base_(base) {};
    ~RecordTypeNode() final = default;

    [[nodiscard]] unsigned int getSize() const final;

    [[nodiscard]] FieldNode *getField(const string &name) const;
    [[nodiscard]] FieldNode *getField(size_t num) const;
    [[nodiscard]] size_t getFieldCount();

    [[nodiscard]] bool isExtened() const;
    [[nodiscard]] bool instanceOf(RecordTypeNode *) const;
    [[nodiscard]] RecordTypeNode *getBaseType() const;

    void accept(NodeVisitor &visitor) final;
    void print(ostream &out) const final;

};


#endif //OBERON0C_RECORDTYPESYMBOL_H
