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
#include "NamedValueNode.h"

class RecordTypeNode final : public TypeNode {

private:
    std::vector<std::unique_ptr<FieldNode>> fields_;

public:
    explicit RecordTypeNode(FilePos pos);
    ~RecordTypeNode() final;

    const int getSize() const final;

    void addField(std::unique_ptr<FieldNode> field);

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_RECORDTYPESYMBOL_H
