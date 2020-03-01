/*
 * AST node representing a procedure in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#ifndef OBERON0C_PROCEDURENODE_H
#define OBERON0C_PROCEDURENODE_H


#include <memory>
#include <vector>
#include "BlockNode.h"
#include "DeclarationNode.h"

class ProcedureNode final : public BlockNode {

private:
    const BlockNode *parent_;
    std::string name_;
    std::vector<std::unique_ptr<ParameterNode>> parameters_;
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;
    bool varargs_;
    bool extern_;

public:
    explicit ProcedureNode(const FilePos &pos, const BlockNode *parent, std::string name, int level) :
            BlockNode(NodeType::procedure, pos, level), parent_(parent), name_(std::move(name)),
            parameters_(), procedures_(), varargs_(false), extern_(false) { };
    ~ProcedureNode() final = default;

    [[nodiscard]] std::string getName() const;

    void addParameter(std::unique_ptr<ParameterNode> parameter);
    [[nodiscard]] ParameterNode* getParameter(size_t num) const;
    [[nodiscard]] size_t getParameterCount() const;

    void addProcedure(std::unique_ptr<ProcedureNode> procedure) final;
    [[nodiscard]] ProcedureNode* getProcedure(size_t num) const final;
    [[nodiscard]] size_t getProcedureCount() const final;

    void setExtern(bool value);
    [[nodiscard]] bool isExtern() const;

    void setVarArgs(bool value);
    [[nodiscard]] bool hasVarArgs() const;

    [[nodiscard]] const BlockNode * getParent() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_PROCEDURENODE_H
