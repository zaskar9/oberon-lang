/*
 * Header file of the AST type nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H


#include <utility>
#include "Node.h"

class TypeNode : public Node {

private:
    std::string name_;
    unsigned int size_;
    bool anon_;

protected:
    void setName(std::string name);

public:
    explicit TypeNode(NodeType nodeType, FilePos pos, std::string name, int size) :
            Node(nodeType, pos), name_(std::move(name)), size_(size), anon_(name.empty()) { };
    ~TypeNode() override;

    void accept(NodeVisitor& visitor) override = 0;

    std::string getName() const;
    virtual unsigned int getSize() const;
    bool isAnonymous();

};


#endif //OBERON0C_TYPENODE_H
