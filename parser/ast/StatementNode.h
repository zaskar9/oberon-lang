/*
 * Header of the AST statement node used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_STATEMENTNODE_H
#define OBERON0C_STATEMENTNODE_H


#include "Node.h"
#include "ExpressionNode.h"

class StatementNode : public Node {

public:
    explicit StatementNode(NodeType type, const FilePos &pos) : Node(type, pos) { };
    ~StatementNode() override = default;

    void accept(NodeVisitor &visitor) override = 0;

};

class ReturnNode : public StatementNode {

private:
    std::unique_ptr<ExpressionNode> value_;

public:
    explicit ReturnNode(const FilePos &pos, std::unique_ptr<ExpressionNode> value) :
            StatementNode(NodeType::ret, pos), value_(std::move(value)) { };
    ~ReturnNode() override = default;

    ExpressionNode * getValue() const;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_STATEMENTNODE_H
