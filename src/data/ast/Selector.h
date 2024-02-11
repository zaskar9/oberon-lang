//
// Created by Michael Grossniklaus on 9/29/22.
//

#ifndef OBERON_LANG_SELECTOR_H
#define OBERON_LANG_SELECTOR_H


#include <memory>
#include <string>
#include <vector>

#include "Ident.h"
#include "DeclarationNode.h"

using std::unique_ptr;
using std::vector;

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
    vector<unique_ptr<ExpressionNode>> indices_;

public:
    explicit ArrayIndex(const FilePos &pos, vector<unique_ptr<ExpressionNode>> indices);
    ~ArrayIndex() override;

    [[nodiscard]] const vector<unique_ptr<ExpressionNode>> &indices() const;

};


class RecordField final : public Selector {

private:
    unique_ptr<Ident> ident_;
    FieldNode *field_;

public:
    explicit RecordField(const FilePos &pos, unique_ptr<Ident> field);
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
    explicit Typeguard(const FilePos &pos, unique_ptr<QualIdent> ident);
    ~Typeguard() override;

    [[nodiscard]] QualIdent* ident() const;

};

class ActualParameters final : public Selector {

private:
    vector<unique_ptr<ExpressionNode>> parameters_;

public:
    explicit ActualParameters(const FilePos &pos, vector<unique_ptr<ExpressionNode>> parameters_);
    ~ActualParameters() override;

    [[nodiscard]] vector<unique_ptr<ExpressionNode>> &parameters();

};


#endif //OBERON_LANG_SELECTOR_H
