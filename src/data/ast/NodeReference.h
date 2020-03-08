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
    virtual ~NodeReference() = default;

    [[nodiscard]] virtual bool isResolved() const = 0;
    [[nodiscard]] virtual Node* dereference() const = 0;

};


class ValueReferenceNode : public ExpressionNode, public NodeReference {

private:
    std::string name_;
    DeclarationNode *node_;
    TypeNode *type_;
    std::vector<std::unique_ptr<ExpressionNode>> selectors_;
    std::vector<NodeType> types_;

public:
    explicit ValueReferenceNode(const NodeType nodeType, const FilePos &pos, std::string name) :
            ExpressionNode(nodeType, pos), NodeReference(),
            name_(std::move(name)), node_(), type_(), selectors_(), types_() { };
    explicit ValueReferenceNode(const FilePos &pos, std::string name) :
            ValueReferenceNode(NodeType::value_reference, pos, std::move(name)) { };
    ~ValueReferenceNode() override = default;

    [[nodiscard]] std::string getName() const;

    [[nodiscard]] bool isResolved() const override;
    void resolve(DeclarationNode *node);
    [[nodiscard]] DeclarationNode* dereference() const override;

    void addSelector(NodeType nodeType, std::unique_ptr<ExpressionNode> selector);
    [[nodiscard]] ExpressionNode* getSelector(size_t num) const;
    [[nodiscard]] NodeType getSelectorType(size_t num) const;
    [[nodiscard]] size_t getSelectorCount() const;

    [[nodiscard]] bool isConstant() const override;

    void setType(TypeNode *type);
    [[nodiscard]] TypeNode* getType() const override;

    void accept(NodeVisitor& visitor) override;

    void print(std::ostream &stream) const override;

};


class TypeReferenceNode final : public TypeNode, public NodeReference {

private:
    TypeNode *node_;

public:
    explicit TypeReferenceNode(const FilePos &pos, std::string name) :
            TypeNode(NodeType::type_reference, pos, std::move(name), 0), NodeReference(), node_() { };
    ~TypeReferenceNode() final = default;

    [[nodiscard]] bool isResolved() const final;
    void resolve(TypeNode *node);
    [[nodiscard]] TypeNode* dereference() const final;

    [[nodiscard]] unsigned int getSize() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


class ProcedureNodeReference : public NodeReference {

private:
    ProcedureNode *procedure_;
    std::vector<std::unique_ptr<ExpressionNode>> parameters_;

public:
    explicit ProcedureNodeReference() : procedure_(), parameters_() { };
    ~ProcedureNodeReference() override = default;

    [[nodiscard]] virtual FilePos pos() const = 0;

    [[nodiscard]] virtual std::string getName() const = 0;

    [[nodiscard]] bool isResolved() const override;
    void resolve(ProcedureNode *procedure);
    [[nodiscard]] ProcedureNode * dereference() const override;

    void addParameter(std::unique_ptr<ExpressionNode> parameter);
    void setParameter(size_t num, std::unique_ptr<ExpressionNode> parameter);
    [[nodiscard]] ExpressionNode * getParameter(size_t num) const;
    [[nodiscard]] size_t getParameterCount() const;

};


class FunctionCallNode final : public ValueReferenceNode, public ProcedureNodeReference {

public:
    explicit FunctionCallNode(const FilePos &pos, std::string name) :
            ValueReferenceNode(NodeType::procedure_call, pos, std::move(name)),
            ProcedureNodeReference() { };
    ~FunctionCallNode() override = default;

    [[nodiscard]] FilePos pos() const override {
        return ValueReferenceNode::pos();
    }

    [[nodiscard]] std::string getName() const override {
        return ValueReferenceNode::getName();
    }

    [[nodiscard]] ProcedureNode * dereference() const override {
        return ProcedureNodeReference::dereference();
    }

    [[nodiscard]] bool isConstant() const final;
    [[nodiscard]] TypeNode * getType() const final;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};

class ProcedureCallNode final : public StatementNode, public ProcedureNodeReference {

private:
    std::string name_;

public:
    ProcedureCallNode(FilePos pos, std::string name) :
            StatementNode(NodeType::procedure_call, pos),
            ProcedureNodeReference(), name_(std::string(name)) { };
    ~ProcedureCallNode() override = default;

    [[nodiscard]] FilePos pos() const override {
        return StatementNode::pos();
    }

    [[nodiscard]] std::string getName() const override;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_REFERENCENODE_H
