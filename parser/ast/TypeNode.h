/*
 * Header file of the AST type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H


#include "Node.h"

class TypeNode : public Node {

private:
    int size_;

public:
    explicit TypeNode(NodeType nodeType, FilePos pos, int size);
    ~TypeNode() override;

    void accept(NodeVisitor& visitor) override = 0;

    virtual int getSize() const;

};


#endif //OBERON0C_TYPENODE_H