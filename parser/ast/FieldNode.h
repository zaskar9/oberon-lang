/*
 * Header file of the AST field declaration nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_VARIABLESYMBOL_H
#define OBERON0C_VARIABLESYMBOL_H


#include <memory>
#include <string>
#include "Node.h"
#include "TypeNode.h"

class FieldNode final : public Node {

private:
    std::string name_;
    std::unique_ptr<const TypeNode> type_;

public:
    FieldNode(FilePos pos, const std::string &name, std::unique_ptr<const TypeNode> type);
    ~FieldNode() final;

    const std::string getName() const;
    const TypeNode* getType() const;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_VARIABLESYMBOL_H
