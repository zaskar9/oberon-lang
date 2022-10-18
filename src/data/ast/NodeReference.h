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
#include "Selector.h"

class NodeReference {

public:
    explicit NodeReference() = default;
    virtual ~NodeReference();

    [[nodiscard]] virtual bool isResolved() const = 0;
    [[nodiscard]] virtual Node *dereference() const = 0;

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
    [[nodiscard]] Selector *getSelector(size_t num) const;
    [[nodiscard]] size_t getSelectorCount() const;

    void unqualify();

};


class ValueReferenceNode : public ExpressionNode, public NodeReference, public Designator {

private:
    DeclarationNode *node_;

protected:
    explicit ValueReferenceNode(const NodeType nodeType, const FilePos &pos, std::unique_ptr<Designator> designator) :
            ExpressionNode(nodeType, pos), Designator(std::move(designator)), node_() {};

public:
    explicit ValueReferenceNode(const FilePos &pos, std::unique_ptr<Designator> designator) :
            ValueReferenceNode(NodeType::value_reference, pos, std::move(designator)) {};
    explicit ValueReferenceNode(const FilePos &pos, DeclarationNode *node) :
            ValueReferenceNode(NodeType::value_reference, pos,
                               std::make_unique<Designator>(std::make_unique<QualIdent>(node->getIdentifier()))) {};
    ~ValueReferenceNode() override = default;

    void resolve(DeclarationNode *node);

    [[nodiscard]] bool isResolved() const override;
    [[nodiscard]] DeclarationNode *dereference() const override;

    [[nodiscard]] bool isConstant() const override;
    [[nodiscard]] int getPrecedence() const final;

    void accept(NodeVisitor &visitor) override;
    void print(std::ostream &stream) const override;

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


class ProcedureNodeReference : public NodeReference {

private:
    ProcedureNode *procedure_;
    std::vector<std::unique_ptr<ExpressionNode>> parameters_;

public:
    explicit ProcedureNodeReference() : procedure_(), parameters_() {};
    ~ProcedureNodeReference() override = default;

    [[nodiscard]] virtual FilePos pos() const = 0;

    [[nodiscard]] virtual QualIdent *ident() const = 0;

    void resolve(ProcedureNode *procedure);

    [[nodiscard]] bool isResolved() const override;
    [[nodiscard]] ProcedureNode *dereference() const override;

    void addActualParameter(std::unique_ptr<ExpressionNode> parameter);
    void setActualParameter(size_t num, std::unique_ptr<ExpressionNode> parameter);
    [[nodiscard]] ExpressionNode *getActualParameter(size_t num) const;
    [[nodiscard]] size_t getActualParameterCount() const;

};


class FunctionCallNode final : public ValueReferenceNode, public ProcedureNodeReference {

public:
    explicit FunctionCallNode(const FilePos &pos, std::unique_ptr<Designator> designator) :
            ValueReferenceNode(NodeType::procedure_call, pos, std::move(designator)),
            ProcedureNodeReference() {};
    ~FunctionCallNode() override = default;

    [[nodiscard]] FilePos pos() const override {
        return ValueReferenceNode::pos();
    }

    [[nodiscard]] QualIdent *ident() const override {
        return ValueReferenceNode::ident();
    }

    [[nodiscard]] ProcedureNode *dereference() const override {
        return ProcedureNodeReference::dereference();
    }

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] TypeNode *getType() const final;

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};

class ProcedureCallNode final : public StatementNode, public ProcedureNodeReference, public Designator {

private:
    std::unique_ptr<Designator> designator_;

public:
    ProcedureCallNode(FilePos pos, std::unique_ptr<Designator> designator) :
            StatementNode(NodeType::procedure_call, pos),
            ProcedureNodeReference(),
            Designator(std::move(designator)) {};
    ~ProcedureCallNode() override = default;

    [[nodiscard]] FilePos pos() const override {
        return StatementNode::pos();
    }

    [[nodiscard]] QualIdent *ident() const override {
        return Designator::ident();
    }

    void accept(NodeVisitor &visitor) final;
    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_REFERENCENODE_H
