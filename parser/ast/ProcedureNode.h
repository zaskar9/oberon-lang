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
#include "ParameterNode.h"

class ProcedureNode final : public BlockNode {

private:
    std::string name_;
    std::vector<std::unique_ptr<const ParameterNode>> parameters_;

public:
    explicit ProcedureNode(FilePos pos, const std::string &name);
    ~ProcedureNode() final;

    const std::string getName() const;
    void addParameter(std::unique_ptr<const ParameterNode> parameter);

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_PROCEDURENODE_H
