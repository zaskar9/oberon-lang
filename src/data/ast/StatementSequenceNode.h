/*
 * AST node representing a statement sequence in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/27/18.
 */

#ifndef OBERON0C_STATEMENTSEQUENCENODE_H
#define OBERON0C_STATEMENTSEQUENCENODE_H


#include "Node.h"
#include "StatementNode.h"
#include <memory>
#include <vector>

class StatementSequenceNode final : public Node {

private:
    std::vector<std::unique_ptr<StatementNode>> statements_;

public:
    explicit StatementSequenceNode(const FilePos &pos) :
            Node(NodeType::statement_sequence, pos), statements_() { };
    ~StatementSequenceNode() override = default;

    void addStatement(std::unique_ptr<StatementNode> statement);
    void insertStatement(size_t pos, std::unique_ptr<StatementNode> statement);
    [[nodiscard]] StatementNode* getStatement(size_t num) const;
    [[nodiscard]] size_t getStatementCount() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_STATEMENTSEQUENCENODE_H
