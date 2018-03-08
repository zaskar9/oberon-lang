//
// Created by Michael Grossniklaus on 3/7/18.
//

#ifndef OBERON0C_NAMEDVALUEDECLARATIONNODE_H
#define OBERON0C_NAMEDVALUEDECLARATIONNODE_H


#include "Node.h"
#include "TypeNode.h"

class NamedValueNode : public Node {

private:
    std::string name_;
    std::shared_ptr<const TypeNode> type_;

public:
    NamedValueNode(NodeType node, FilePos pos, const std::string &name, const std::shared_ptr<const TypeNode> &type);
    ~NamedValueNode() override;

    const std::string getName() const;
    std::shared_ptr<const TypeNode> getType() const;

    void print(std::ostream &stream) const;

};


#endif //OBERON0C_NAMEDVALUEDECLARATIONNODE_H
