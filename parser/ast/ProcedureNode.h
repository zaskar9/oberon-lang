/*
 * Header file of the AST procedure nodes used by the Oberon-0 compiler.
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
    std::string name_;
    std::vector<std::unique_ptr<ParameterNode>> parameters_;
    std::vector<std::unique_ptr<ProcedureNode>> procedures_;

public:
    explicit ProcedureNode(const FilePos pos, const std::string &name, int level) :
            BlockNode(NodeType::procedure, pos, level), name_(name), parameters_(), procedures_() { };
    ~ProcedureNode() final = default;

    const std::string getName() const;

    void addParameter(std::unique_ptr<ParameterNode> parameter);
    ParameterNode* getParameter(size_t num) const;
    size_t getParameterCount() const;

    void addProcedure(std::unique_ptr<ProcedureNode> procedure) final;
    ProcedureNode* getProcedure(size_t num) const final;
    size_t getProcedureCount() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_PROCEDURENODE_H