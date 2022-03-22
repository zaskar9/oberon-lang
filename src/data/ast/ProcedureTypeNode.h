//
// Created by Michael Grossniklaus on 3/17/22.
//

#ifndef OBERON_LANG_PROCEDURETYPENODE_H
#define OBERON_LANG_PROCEDURETYPENODE_H


#include <vector>
#include "TypeNode.h"
#include "DeclarationNode.h"

class ProcedureTypeNode final : public TypeNode {

private:
    std::vector<std::unique_ptr<ParameterNode>> parameters_;
    bool varargs_;
    TypeNode *type_;

public:
    explicit ProcedureTypeNode(const FilePos &pos) :
            TypeNode(NodeType::procedure_type, pos, nullptr, TypeKind::PROCEURE, 0),
            parameters_(), varargs_(false), type_(nullptr) {};
    ~ProcedureTypeNode() override = default;

    void addParameter(std::unique_ptr<ParameterNode> parameter);
    [[nodiscard]] ParameterNode *getParameter(const std::string &name);
    [[nodiscard]] ParameterNode *getParameter(size_t num) const;
    [[nodiscard]] size_t getParameterCount() const;

    void setVarArgs(bool value);
    [[nodiscard]] bool hasVarArgs() const;

    void setReturnType(TypeNode *type);
    [[nodiscard]] TypeNode *getReturnType() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON_LANG_PROCEDURETYPENODE_H
