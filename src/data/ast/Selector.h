//
// Created by Michael Grossniklaus on 9/29/22.
//

#ifndef OBERON_LANG_SELECTOR_H
#define OBERON_LANG_SELECTOR_H


#include "Ident.h"
#include "DeclarationNode.h"
#include <memory>
#include <string>
#include <vector>

class Selector {

private:
    NodeType type_;
    FilePos pos_;

public:
    explicit Selector(NodeType type, const FilePos &pos) : type_(type), pos_(pos) {};
    virtual ~Selector();

    [[nodiscard]] FilePos pos() const;
    [[nodiscard]] NodeType getType() const;
};

class ExpressionNode;

class ArrayIndex final : public Selector {

private:
    std::unique_ptr<ExpressionNode> expression_;

public:
    explicit ArrayIndex(const FilePos &pos, std::unique_ptr<ExpressionNode> expression);
    ~ArrayIndex() override;

    [[nodiscard]] ExpressionNode *getExpression() const;
};


class RecordField final : public Selector {

private:
    std::unique_ptr<Ident> ident_;
    FieldNode *field_;

public:
    explicit RecordField(const FilePos &pos, std::unique_ptr<Ident> field);
    explicit RecordField(const FilePos &pos, FieldNode *field);
    ~RecordField() override;

    [[nodiscard]] Ident *ident() const;
    void setField(FieldNode *field);
    [[nodiscard]] FieldNode *getField() const;

};


class Dereference final : public Selector {

public:
    explicit Dereference(const FilePos &pos);
    ~Dereference() override;

};


class Typeguard final : public Selector {

private:
    std::unique_ptr<QualIdent> ident_;

public:
    explicit Typeguard(const FilePos &pos, std::unique_ptr<QualIdent> ident);
    ~Typeguard() override;

    [[nodiscard]] QualIdent* ident() const;

};

class ActualParameters final : public Selector {

private:
    std::vector<std::unique_ptr<ExpressionNode>> parameters_;

public:
    explicit ActualParameters(const FilePos &pos);
    ~ActualParameters() override;

    void addActualParameter(std::unique_ptr<ExpressionNode> parameter);
    void moveActuralParameters(std::vector<std::unique_ptr<ExpressionNode>> &target);
};


#endif //OBERON_LANG_SELECTOR_H
