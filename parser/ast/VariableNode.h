/*
 * Header file of the AST variable declaration nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/2/18.
 */

#ifndef OBERON0C_VARIABLENODE_H
#define OBERON0C_VARIABLENODE_H


#include "TypeNode.h"
#include "ExpressionNode.h"

class VariableNode final : public ExpressionNode {

private:
    std::string name_;
    std::unique_ptr<const TypeNode> type_;

public:
    VariableNode(FilePos pos, const std::string &name, std::unique_ptr<const TypeNode> type);
    ~VariableNode() final;

    const std::string getName() const;
    const TypeNode* getType() const;

    bool isConstant() const final;
    ExpressionType checkType() const final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_VARIABLENODE_H
