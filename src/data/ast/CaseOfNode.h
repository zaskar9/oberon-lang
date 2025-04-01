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

class CaseLabelNode : public Node {

private:
    vector<unique_ptr<ExpressionNode>> labels_;
    set<int64_t> values_;

public:
    CaseLabelNode(const FilePos &pos, vector<std::unique_ptr<ExpressionNode>> &&labels, set<int64_t> &&values) :
        Node(NodeType::case_label, pos), labels_(std::move(labels)), values_(values) {};
    ~CaseLabelNode() override = default;

    [[nodiscard]] TypeNode* getType() const;

    [[nodiscard]] ExpressionNode* getValue(size_t num) const;
    [[nodiscard]] size_t getValueCount() const;
    [[nodiscard]] const set<int64_t> &getValues() const;

    void accept(NodeVisitor&) override;

    void print(ostream &) const override;

};

class CaseNode final : public Node {

private:
    unique_ptr<CaseLabelNode> label_;
    unique_ptr<StatementSequenceNode> statements_;

public:
    CaseNode(const FilePos &pos, unique_ptr<CaseLabelNode> label, unique_ptr<StatementSequenceNode> stmts) :
            Node(NodeType::case_case, pos), label_(std::move(label)), statements_(std::move(stmts)) {}
    ~CaseNode() override = default;

    [[nodiscard]] CaseLabelNode* getLabel() const;
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
    ~CaseOfNode() final = default;

    [[nodiscard]] ExpressionNode* getExpression() const;

    [[nodiscard]] CaseNode* getCase(size_t) const;
    [[nodiscard]] size_t getCaseCount() const;

    [[nodiscard]] size_t getLabelCount() const;

    [[nodiscard]] StatementSequenceNode* getElseStatements() const;
    [[nodiscard]] bool hasElse() const;

    [[nodiscard]] bool hasExit() final;
    [[nodiscard]] bool isReturn() final;
    [[nodiscard]] bool isTerminator() final;

    void accept(NodeVisitor&) override;

    void print(ostream &) const override;

};


#endif //CASEOFNODE_H
