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
    bool exit_, return_, term_;
    size_t termIdx_;

    void updateState(size_t, StatementNode *);

public:
    explicit StatementSequenceNode(const FilePos &pos) :
            Node(NodeType::statement_sequence, pos),
            statements_(), exit_(false), return_(false), term_(false), termIdx_(0) {}
    ~StatementSequenceNode() final = default;

    void addStatement(unique_ptr<StatementNode>);
    void insertStatement(size_t, unique_ptr<StatementNode>);
    [[nodiscard]] StatementNode* getStatement(size_t) const;
    [[nodiscard]] size_t getStatementCount() const;

    [[nodiscard]] bool hasExit() const;
    [[nodiscard]] bool isReturn() const;
    [[nodiscard]] bool hasTerminator() const;
    [[nodiscard]] size_t getTerminatorIndex() const;

    void accept(NodeVisitor &) final;

    void print(std::ostream &) const final;

};

#endif //OBERON0C_STATEMENTSEQUENCENODE_H
