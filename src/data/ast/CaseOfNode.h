//
// Created by Michael Grossniklaus on 1/11/25.
//

#ifndef CASEOFNODE_H
#define CASEOFNODE_H


#include <iostream>
#include <memory>
#include <set>
#include <utility>

#include "Node.h"
#include "StatementNode.h"
#include "ExpressionNode.h"
#include "StatementSequenceNode.h"

using std::ostream;
using std::set;
using std::unique_ptr;

class CaseNode final : public Node {

private:
    vector<std::unique_ptr<ExpressionNode>> labels_;
    set<int64_t> cases_;
    unique_ptr<StatementSequenceNode> statements_;

public:
    CaseNode(const FilePos &pos, vector<std::unique_ptr<ExpressionNode>> &&labels, set<int64_t> &&cases,
             unique_ptr<StatementSequenceNode> stmts) :
        Node(NodeType::case_case, pos), labels_(std::move(labels)), cases_(cases), statements_(std::move(stmts)) {}
    ~CaseNode() override = default;

    [[nodiscard]] const set<int64_t> &getCases() const;

    [[nodiscard]] ExpressionNode* getLabel(size_t num) const;
    [[nodiscard]] size_t getLabelCount() const;

    [[nodiscard]] TypeNode* getLabelType() const;

    [[nodiscard]] StatementSequenceNode* getStatements() const;

    void accept(NodeVisitor&) override;

    void print(ostream &) const override;

};

class CaseOfNode final : public StatementNode {

private:
    unique_ptr<ExpressionNode> expression_;
    vector<unique_ptr<CaseNode>> cases_;
    unique_ptr<StatementSequenceNode> elseStatements_;
    mutable size_t labels_;

public:
    CaseOfNode(const FilePos &pos, unique_ptr<ExpressionNode> expr, vector<unique_ptr<CaseNode>> &&cases,
        unique_ptr<StatementSequenceNode> elseStmts) : StatementNode(NodeType::case_of, pos),
        expression_(std::move(expr)), cases_(std::move(cases)), elseStatements_(std::move(elseStmts)), labels_() {}
    ~CaseOfNode() override = default;

    [[nodiscard]] ExpressionNode* getExpression() const;

    [[nodiscard]] CaseNode* getCase(size_t) const;
    [[nodiscard]] size_t getCaseCount() const;

    [[nodiscard]] size_t getLabelCount() const;

    [[nodiscard]] StatementSequenceNode* getElseStatements() const;
    [[nodiscard]] bool hasElse() const;

    void accept(NodeVisitor&) override;

    void print(ostream &) const override;

};


#endif //CASEOFNODE_H
