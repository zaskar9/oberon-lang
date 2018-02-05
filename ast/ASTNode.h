//
// Created by Michael Grossniklaus on 2/2/18.
//

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H

#include <list>
#include <string>

enum class NodeType : char {

};

class ASTNode {

private:
    std::list<ASTNode*> _children;

public:
    explicit ASTNode(NodeType type);
    virtual ~ASTNode();
    void addChild(ASTNode* child);
    std::list<ASTNode*> getChildren() const;

};


#endif //OBERON0C_AST_H
