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

private:
    vector<unique_ptr<ParameterNode>> parameters_;
    bool varargs_;
    TypeNode *type_;

public:
    ProcedureTypeNode() : ProcedureTypeNode(EMPTY_POS, nullptr) {};
    ProcedureTypeNode(const FilePos &pos, Ident *ident) :
            TypeNode(NodeType::procedure_type, pos, ident, TypeKind::PROCEDURE, 0),
            parameters_(), varargs_(false), type_(nullptr) {};
    ProcedureTypeNode(Ident *ident, vector<unique_ptr<ParameterNode>> params, TypeNode *type) :
            TypeNode(NodeType::procedure_type, EMPTY_POS, ident, TypeKind::PROCEDURE, 0),
            parameters_(std::move(params)), varargs_(false), type_(type) {};
    ~ProcedureTypeNode() override = default;

    void addFormalParameter(unique_ptr<ParameterNode> parameter);
    [[nodiscard]] ParameterNode *getFormalParameter(const string &name);
    [[nodiscard]] ParameterNode *getFormalParameter(size_t num) const;
    [[nodiscard]] size_t getFormalParameterCount() const;

    void setVarArgs(bool value);
    [[nodiscard]] bool hasVarArgs() const;

    void setReturnType(TypeNode *type);
    [[nodiscard]] TypeNode *getReturnType() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &out) const final;

};


#endif //OBERON_LANG_PROCEDURETYPENODE_H
