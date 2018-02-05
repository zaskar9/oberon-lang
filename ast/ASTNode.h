/*
 * Header file of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H

#include <list>
#include <string>

enum class NodeType : char {
    module, const_declaration, type_declaration, var_declaration, procedure_declaration
};

class ASTNode {

private:
    NodeType type_;
    ASTNode *next_;
    ASTNode *firstChild_, *lastChild_;

    void setNext(ASTNode *next);

public:
    explicit ASTNode(const NodeType type);
    virtual ~ASTNode();
    const NodeType getNodeType() const;
    const ASTNode* getNext() const;
    const ASTNode* getFirstChild() const;
    void addChild(ASTNode *child);

};

#endif //OBERON0C_AST_H
