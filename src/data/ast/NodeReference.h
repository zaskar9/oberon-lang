/*
 * AST node representing a reference to a declaration in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_REFERENCENODE_H
#define OBERON0C_REFERENCENODE_H


#include <memory>
#include <vector>

#include "ExpressionNode.h"
#include "DeclarationNode.h"
#include "ProcedureNode.h"
#include "Designator.h"

using std::unique_ptr;
using std::vector;

class NodeReference {

private:
    DeclarationNode* node_;

public:
    NodeReference() : node_() {};
    explicit NodeReference(DeclarationNode *node) : node_(node) {};
    virtual ~NodeReference();

    virtual void resolve(DeclarationNode *node);
    [[nodiscard]] virtual bool isResolved() const;
    [[nodiscard]] virtual DeclarationNode *dereference() const;

};


class ProcedureNodeReference : public NodeReference, public Designator {

private:
    vector<unique_ptr<ExpressionNode>> parameters_;

public:
    explicit ProcedureNodeReference(unique_ptr<Designator> designator) :
            Designator(std::move(designator)),
            parameters_() {};
    ~ProcedureNodeReference() override;

    [[nodiscard]] virtual FilePos pos() = 0;

    void addActualParameter(unique_ptr<ExpressionNode> parameter);
    void setActualParameter(size_t num, unique_ptr<ExpressionNode> parameter);
    [[nodiscard]] ExpressionNode *getActualParameter(size_t num) const;
    [[nodiscard]] size_t getActualParameterCount() const;

protected:
    void initActualParameters();

};


class ValueReferenceNode final : public ExpressionNode, public ProcedureNodeReference {

public:
    // ctor for parser / sema
    ValueReferenceNode(const FilePos &pos, std::unique_ptr<Designator> designator) :
            ExpressionNode(NodeType::value_reference, pos, nullptr),
            ProcedureNodeReference(std::move(designator)) {};
    // ctor for transformer
    ValueReferenceNode(const FilePos &pos, DeclarationNode *node);
    ~ValueReferenceNode() override = default;

    [[nodiscard]] FilePos pos() override { return ExpressionNode::pos(); };

    void resolve(DeclarationNode *node) override;

    [[nodiscard]] bool isConstant() const override;
    [[nodiscard]] int getPrecedence() const override;
    [[nodiscard]] TypeNode* getType() const override;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


class ProcedureCallNode final : public StatementNode, public ProcedureNodeReference {

public:
    ProcedureCallNode(const FilePos& pos, std::unique_ptr<Designator> designator) :
            StatementNode(NodeType::procedure_call, pos),
            ProcedureNodeReference(std::move(designator)) {};
    ~ProcedureCallNode() override = default;

    [[nodiscard]] FilePos pos() override { return StatementNode::pos(); };

    void resolve(DeclarationNode *node) override;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


class QualifiedExpression : public ExpressionNode, public Designator, public NodeReference {

public:
    QualifiedExpression(const FilePos &pos, unique_ptr<Designator> designator, DeclarationNode *decl, TypeNode *type) :
            ExpressionNode(NodeType::qualified_expression, pos, type),
            Designator(std::move(designator)),
            NodeReference(decl) {};
    QualifiedExpression(DeclarationNode *decl) :
            ExpressionNode(NodeType::qualified_expression, EMPTY_POS, decl->getType()),
            Designator(make_unique<Designator>(make_unique<QualIdent>(decl->getIdentifier()))),
            NodeReference(decl) {};
    ~QualifiedExpression();

    [[nodiscard]] bool isConstant() const override;
    [[nodiscard]] int getPrecedence() const override;
    [[nodiscard]] TypeNode* getType() const override;

    void resolve(DeclarationNode *node) override;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


class QualifiedStatement : public StatementNode, public Designator, public NodeReference {

public:
    QualifiedStatement(const FilePos &pos, unique_ptr<Designator> designator, DeclarationNode *decl) :
            StatementNode(NodeType::qualified_statement, pos),
            Designator(std::move(designator)),
            NodeReference(decl) {};
    ~QualifiedStatement();

    // void accept(NodeVisitor &visitor) override;
    // void print(std::ostream &stream) const override;

};


#endif //OBERON0C_REFERENCENODE_H
