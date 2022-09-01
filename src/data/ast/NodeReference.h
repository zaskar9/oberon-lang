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

class NodeReference {

public:
    explicit NodeReference() = default;
    virtual ~NodeReference();

    [[nodiscard]] virtual bool isResolved() const = 0;
    [[nodiscard]] virtual Node *dereference() const = 0;

};


class ValueReferenceNode : public ExpressionNode, public NodeReference {

private:
    std::unique_ptr<Identifier> ident_;
    DeclarationNode *node_;
    std::vector<std::unique_ptr<ExpressionNode>> selectors_;
    std::vector<NodeType> types_;

public:
    explicit ValueReferenceNode(const NodeType nodeType, const FilePos &pos, std::unique_ptr<Identifier> ident) :
            ExpressionNode(nodeType, pos), NodeReference(),
            ident_(std::move(ident)), node_(), selectors_(), types_() {};

    explicit ValueReferenceNode(const FilePos &pos, std::unique_ptr<Identifier> ident) :
            ValueReferenceNode(NodeType::value_reference, pos, std::move(ident)) {};

    explicit ValueReferenceNode(const FilePos &pos, DeclarationNode *node) :
            ExpressionNode(NodeType::value_reference, pos, node->getType()), NodeReference(),
            ident_(std::make_unique<Identifier>(node->getIdentifier())),
            node_(node), selectors_(), types_() {};
    ~ValueReferenceNode() override = default;

    [[nodiscard]] Identifier *getIdentifier() const;

    [[nodiscard]] bool isResolved() const override;
    void resolve(DeclarationNode *node);
    [[nodiscard]] DeclarationNode *dereference() const override;

    void addSelector(NodeType nodeType, std::unique_ptr<ExpressionNode> selector);
    void insertSelector(size_t num, NodeType nodeType, std::unique_ptr<ExpressionNode> selector);
    void setSelector(size_t num, std::unique_ptr<ExpressionNode> selector);
    [[nodiscard]] ExpressionNode *getSelector(size_t num) const;
    [[nodiscard]] NodeType getSelectorType(size_t num) const;
    [[nodiscard]] size_t getSelectorCount() const;

    [[nodiscard]] bool isConstant() const override;

    [[nodiscard]] int getPrecedence() const final;

    void accept(NodeVisitor &visitor) override;

    void print(std::ostream &stream) const override;

};


class TypeReferenceNode final : public TypeNode, public NodeReference {

private:
    std::unique_ptr<Identifier> ident_; // required for memory management
    TypeNode *node_;

public:
    explicit TypeReferenceNode(const FilePos &pos, std::unique_ptr<Identifier> ident) :
            TypeNode(NodeType::type_reference, pos, ident.get(), TypeKind::NOTYPE, 0),
            NodeReference(), ident_(std::move(ident)), node_() {};
    ~TypeReferenceNode() final = default;

    [[nodiscard]] bool isResolved() const final;
    void resolve(TypeNode *node);
    [[nodiscard]] TypeNode *dereference() const final;

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

    [[nodiscard]] virtual Identifier *getIdentifier() const = 0;

    [[nodiscard]] bool isResolved() const override;
    void resolve(ProcedureNode *procedure);
    [[nodiscard]] ProcedureNode *dereference() const override;

    void addActualParameter(std::unique_ptr<ExpressionNode> parameter);
    void setActualParameter(size_t num, std::unique_ptr<ExpressionNode> parameter);
    [[nodiscard]] ExpressionNode *getActualParameter(size_t num) const;
    [[nodiscard]] size_t getActualParameterCount() const;

};


class FunctionCallNode final : public ValueReferenceNode, public ProcedureNodeReference {

public:
    explicit FunctionCallNode(const FilePos &pos, std::unique_ptr<Identifier> ident) :
            ValueReferenceNode(NodeType::procedure_call, pos, std::move(ident)),
            ProcedureNodeReference() {};
    ~FunctionCallNode() override = default;

    [[nodiscard]] FilePos pos() const override {
        return ValueReferenceNode::pos();
    }

    [[nodiscard]] Identifier *getIdentifier() const override {
        return ValueReferenceNode::getIdentifier();
    }

    [[nodiscard]] ProcedureNode *dereference() const override {
        return ProcedureNodeReference::dereference();
    }

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] TypeNode *getType() const final;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};

class ProcedureCallNode final : public StatementNode, public ProcedureNodeReference {

private:
    std::unique_ptr<Identifier> ident_;

public:
    ProcedureCallNode(FilePos pos, std::unique_ptr<Identifier> ident) :
            StatementNode(NodeType::procedure_call, pos),
            ProcedureNodeReference(), ident_(std::move(ident)) {};
    ~ProcedureCallNode() override = default;

    [[nodiscard]] FilePos pos() const override {
        return StatementNode::pos();
    }

    [[nodiscard]] Identifier *getIdentifier() const override;

    void accept(NodeVisitor &visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_REFERENCENODE_H
