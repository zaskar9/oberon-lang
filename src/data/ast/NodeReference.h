/*
 * AST node representing a reference to a declaration in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_REFERENCENODE_H
#define OBERON0C_REFERENCENODE_H


#include "ExpressionNode.h"
#include "DeclarationNode.h"
#include "ProcedureNode.h"
#include "Selector.h"
#include <memory>
#include <vector>

class NodeReference {

public:
    explicit NodeReference() = default;
    virtual ~NodeReference();

    [[nodiscard]] virtual bool isResolved() const = 0;
    [[nodiscard]] virtual Node *dereference() const = 0;

};


class TypeReferenceNode final : public TypeNode, public NodeReference {

private:
    std::unique_ptr<Ident> ident_; // required for memory management
    TypeNode *node_;

public:
    explicit TypeReferenceNode(const FilePos &pos, std::unique_ptr<Ident> ident) :
            TypeNode(NodeType::type_reference, pos, ident.get(), TypeKind::NOTYPE, 0),
            NodeReference(), ident_(std::move(ident)), node_() {};
    ~TypeReferenceNode() final = default;

    [[nodiscard]] bool isResolved() const final;
    [[nodiscard]] TypeNode *dereference() const final;

    void resolve(TypeNode *node);

    [[nodiscard]] TypeKind kind() const final;
    [[nodiscard]] unsigned int getSize() const final;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


class Selector;

class Designator {

private:
    std::unique_ptr<QualIdent> ident_;
    std::vector<std::unique_ptr<Selector>> selectors_;

public:
    explicit Designator(std::unique_ptr<QualIdent> ident) :
            ident_(std::move(ident)), selectors_() {};
    explicit Designator(std::unique_ptr<Ident> ident) :
            ident_(std::make_unique<QualIdent>(ident.get())), selectors_() {};
    explicit Designator(std::unique_ptr<Designator> &&designator) :
            ident_(std::move(designator->ident_)), selectors_(std::move(designator->selectors_)) {};
    virtual ~Designator();

    [[nodiscard]] QualIdent *ident() const;

    void addSelector(std::unique_ptr<Selector> selector);
    void insertSelector(size_t num, std::unique_ptr<Selector> selector);
    void setSelector(size_t num, std::unique_ptr<Selector> selector);
    void removeSelector(size_t num);
    [[nodiscard]] Selector *getSelector(size_t num) const;
    [[nodiscard]] size_t getSelectorCount() const;

    void disqualify();

};


class ProcedureNodeReference : public NodeReference, public Designator {

private:
    std::vector<std::unique_ptr<ExpressionNode>> parameters_;

public:
    explicit ProcedureNodeReference(std::unique_ptr<Designator> designator) :
            Designator(std::move(designator)),
            parameters_() {};
    ~ProcedureNodeReference() override;

    [[nodiscard]] virtual FilePos pos() = 0;

    virtual void resolve(DeclarationNode *node) = 0;

    void addActualParameter(std::unique_ptr<ExpressionNode> parameter);
    void setActualParameter(size_t num, std::unique_ptr<ExpressionNode> parameter);
    [[nodiscard]] ExpressionNode *getActualParameter(size_t num) const;
    [[nodiscard]] size_t getActualParameterCount() const;

protected:
    void initActualParameters();

};


class ValueReferenceNode final : public ExpressionNode, public ProcedureNodeReference {

private:
    DeclarationNode *node_;

public:
    explicit ValueReferenceNode(const FilePos &pos, std::unique_ptr<Designator> designator) :
            ExpressionNode(NodeType::value_reference, pos),
            ProcedureNodeReference(std::move(designator)),
            node_() {};
    explicit ValueReferenceNode(const FilePos &pos, DeclarationNode *node);
    ~ValueReferenceNode() override = default;

    [[nodiscard]] FilePos pos() override { return ExpressionNode::pos(); };

    void resolve(DeclarationNode *node) override;

    [[nodiscard]] bool isResolved() const override;
    [[nodiscard]] DeclarationNode *dereference() const override;

    [[nodiscard]] bool isConstant() const override;
    [[nodiscard]] int getPrecedence() const override;
    [[nodiscard]] TypeNode* getType() const override;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


class ProcedureCallNode final : public StatementNode, public ProcedureNodeReference {

private:
    ProcedureNode *node_;

public:
    ProcedureCallNode(FilePos pos, std::unique_ptr<Designator> designator) :
            StatementNode(NodeType::procedure_call, pos),
            ProcedureNodeReference(std::move(designator)),
            node_() {};
    ~ProcedureCallNode() override = default;

    [[nodiscard]] FilePos pos() override { return StatementNode::pos(); };

    void resolve(DeclarationNode *node) override;

    [[nodiscard]] bool isResolved() const override;
    [[nodiscard]] ProcedureNode *dereference() const override;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

};


#endif //OBERON0C_REFERENCENODE_H
