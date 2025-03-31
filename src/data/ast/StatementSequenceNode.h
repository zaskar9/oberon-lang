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

using std::unique_ptr;
using std::vector;

class StatementSequenceNode final : public Node {

private:
    vector<unique_ptr<StatementNode>> statements_;
    bool exit_;
    bool return_;
    size_t retIdx_;

public:
    explicit StatementSequenceNode(const FilePos &pos) :
            Node(NodeType::statement_sequence, pos), statements_(), exit_(false), return_(false), retIdx_(0) { };
    ~StatementSequenceNode() final = default;

    void addStatement(unique_ptr<StatementNode> statement);
    void insertStatement(size_t pos, unique_ptr<StatementNode> statement);
    [[nodiscard]] StatementNode* getStatement(size_t num) const;
    [[nodiscard]] size_t getStatementCount() const;

    [[nodiscard]] bool hasExit();
    [[nodiscard]] bool isReturn();
    [[nodiscard]] size_t getReturnIndex();

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

#endif //OBERON0C_STATEMENTSEQUENCENODE_H
