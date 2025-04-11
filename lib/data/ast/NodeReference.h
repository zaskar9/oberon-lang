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

    [[nodiscard]] virtual FilePos pos() const = 0;

    virtual void resolve(DeclarationNode *node);
    [[nodiscard]] virtual DeclarationNode *dereference() const;

};


class QualifiedExpression : public ExpressionNode, public Designator, public NodeReference {

public:
    QualifiedExpression(const FilePos &pos, unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors,
                        DeclarationNode *decl, TypeNode *type) :
            ExpressionNode(NodeType::qualified_expression, pos, type),
            Designator(std::move(ident), std::move(selectors)),
            NodeReference(decl) {};
    explicit QualifiedExpression(DeclarationNode *decl) :
            ExpressionNode(NodeType::qualified_expression, EMPTY_POS, decl->getType()),
            Designator(make_unique<QualIdent>(decl->getIdentifier())),
            NodeReference(decl) {};
    ~QualifiedExpression() override;

    [[nodiscard]] FilePos pos() const override;

    [[nodiscard]] bool isConstant() const override;
    [[nodiscard]] int getPrecedence() const override;
    [[nodiscard]] TypeNode* getType() const override;

    void resolve(DeclarationNode *node) override;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


class QualifiedStatement : public StatementNode, public Designator, public NodeReference {

public:
    QualifiedStatement(const FilePos &pos, unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors,
                       DeclarationNode *decl) :
            StatementNode(NodeType::qualified_statement, pos),
            Designator(std::move(ident), std::move(selectors)),
            NodeReference(decl) {};
    ~QualifiedStatement() override;

    [[nodiscard]] FilePos pos() const override;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_REFERENCENODE_H
