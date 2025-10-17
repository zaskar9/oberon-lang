//
// Created by Michael Grossniklaus on 3/17/22.
//

#ifndef OBERON_LANG_PROCEDURETYPENODE_H
#define OBERON_LANG_PROCEDURETYPENODE_H


#include <memory>
#include <string>
#include <vector>

#include "TypeNode.h"
#include "DeclarationNode.h"

using std::unique_ptr;
using std::string;
using std::vector;

class ProcedureTypeNode final : public TypeNode {

public:
    ProcedureTypeNode(const FilePos &pos, vector<unique_ptr<ParameterNode>> params, const bool varargs, TypeNode *type) :
            TypeNode(NodeType::procedure_type, pos, TypeKind::PROCEDURE, 0),
            parameters_(std::move(params)), varargs_(varargs), type_(type) {}
    ~ProcedureTypeNode() override = default;

    [[nodiscard]] vector<unique_ptr<ParameterNode>> &parameters();

    [[nodiscard]] bool hasVarArgs() const;

    void setReturnType(TypeNode *);
    [[nodiscard]] TypeNode *getReturnType() const;
    [[nodiscard]] bool isProper() const;
    [[nodiscard]] bool isFunction() const;

    void accept(NodeVisitor &visitor) override;

    void print(std::ostream &out) const override;

private:
    vector<unique_ptr<ParameterNode>> parameters_;
    bool varargs_;
    TypeNode *type_;

};


#endif //OBERON_LANG_PROCEDURETYPENODE_H
