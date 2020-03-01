/*
 * Header file of the AST record type nodes used by the Oberon-0 compiler.
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
    int offset_;
    std::vector<std::unique_ptr<FieldNode>> fields_;

public:
    explicit RecordTypeNode(FilePos pos, const std::string &name) :
            TypeNode(NodeType::record_type, pos, name, 0), offset_(0), fields_() { };
    ~RecordTypeNode() final = default;

    unsigned int getSize() const final;

    int getOffset() const;
    void incOffset(int offset);

    void addField(std::unique_ptr<FieldNode> field);
    FieldNode* getField(const std::string &name) const;
    FieldNode* getField(size_t num) const;
    size_t getFieldCount();

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_RECORDTYPESYMBOL_H
