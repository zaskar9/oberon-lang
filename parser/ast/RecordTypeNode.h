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
#include "FieldNode.h"

class RecordTypeNode : public TypeNode {

private:
    std::vector<std::unique_ptr<const FieldNode>> fields_;

public:
    explicit RecordTypeNode(FilePos pos);
    ~RecordTypeNode() override;

    void addField(std::unique_ptr<const FieldNode> field);
    const int getSize() const override;

    void print(std::ostream &stream) const override;

};

#endif //OBERON0C_RECORDTYPESYMBOL_H
