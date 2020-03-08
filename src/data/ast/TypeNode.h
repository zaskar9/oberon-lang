/*
 * AST node representing a type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#ifndef OBERON0C_TYPENODE_H
#define OBERON0C_TYPENODE_H


#include <utility>
#include "Node.h"

class TypeNode : public Node {

private:
    const std::string name_;
    unsigned int size_;
    const bool anon_;

public:
    explicit TypeNode(NodeType nodeType, const FilePos &pos, std::string name, unsigned int size) :
            Node(nodeType, pos), name_(std::move(name)), size_(size), anon_(name_.empty()) { };
    ~TypeNode() override = default;

    [[nodiscard]] std::string getName() const;

    void setSize(unsigned int size);
    [[nodiscard]] virtual unsigned int getSize() const;

    [[nodiscard]] bool isAnonymous();

    void accept(NodeVisitor& visitor) override = 0;

};


#endif //OBERON0C_TYPENODE_H
