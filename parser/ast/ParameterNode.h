/*
 * Header file of the AST parameter declaration nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/5/18.
 */

#ifndef OBERON0C_PARAMETERNODE_H
#define OBERON0C_PARAMETERNODE_H


#include <memory>
#include <string>
#include "TypeNode.h"

class ParameterNode final : public Node {

private:
    std::string name_;
    std::unique_ptr<const TypeNode> type_;
    bool var_;

public:
    explicit ParameterNode(FilePos pos, const std::string &name, std::unique_ptr<const TypeNode> type, bool var);
    ~ParameterNode() final;

    const std::string getName() const;
    const TypeNode* getType() const;
    const bool isVar() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_PARAMETERNODE_H
