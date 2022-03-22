/*
 * AST node representing a record type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_RECORDTYPESYMBOL_H
#define OBERON0C_RECORDTYPESYMBOL_H


#include <memory>
#include <vector>
#include "TypeNode.h"
#include "DeclarationNode.h"

class RecordTypeNode final : public TypeNode {

private:
    std::vector<std::unique_ptr<FieldNode>> fields_;
    RecordTypeNode *base_;

public:
    explicit RecordTypeNode(const FilePos &pos, Identifier *ident) :
            TypeNode(NodeType::record_type, pos, ident, TypeKind::RECORD, 0), fields_(), base_() {};
    ~RecordTypeNode() final = default;

    [[nodiscard]] unsigned int getSize() const final;

    void addField(std::unique_ptr<FieldNode> field);
    [[nodiscard]] FieldNode *getField(const std::string &name) const;
    [[nodiscard]] FieldNode *getField(size_t num) const;
    [[nodiscard]] size_t getFieldCount();

    void setBaseType(RecordTypeNode *base);
    [[nodiscard]] RecordTypeNode *getBaseType() const;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_RECORDTYPESYMBOL_H
