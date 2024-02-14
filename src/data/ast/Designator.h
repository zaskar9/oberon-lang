//
// Created by Michael Grossniklaus on 9/29/22.
//

#ifndef OBERON_LANG_DESIGNATOR_H
#define OBERON_LANG_DESIGNATOR_H


#include <memory>
#include <string>
#include <vector>

#include "Ident.h"
#include "DeclarationNode.h"


using std::make_unique;
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
    [[nodiscard]] NodeType getNodeType() const;
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
    unique_ptr<QualIdent> ident_;
    TypeNode *type_;

public:
    explicit Typeguard(const FilePos &pos, unique_ptr<QualIdent> ident);
    ~Typeguard() override;

    [[nodiscard]] QualIdent *ident() const;
    void setType(TypeNode *);
    [[nodiscard]] TypeNode *getType() const;

};

class ProcedureNode;

class ActualParameters final : public Selector {

private:
    ProcedureNode *proc_;
    vector<unique_ptr<ExpressionNode>> parameters_;

public:
    explicit ActualParameters(const FilePos &pos, vector<unique_ptr<ExpressionNode>> parameters_);
    ~ActualParameters() override;

    void setProcedure(ProcedureNode *);
    [[nodiscard]] ProcedureNode *getProcedure() const;

    [[nodiscard]] vector<unique_ptr<ExpressionNode>> &parameters();

};


class Designator {

private:
    unique_ptr<QualIdent> ident_;
    vector<unique_ptr<Selector>> selectors_;

public:
    explicit Designator(unique_ptr<QualIdent> ident) :
            ident_(std::move(ident)), selectors_() {};
    explicit Designator(unique_ptr<Ident> ident) :
            ident_(make_unique<QualIdent>(ident.get())), selectors_() {};
    explicit Designator(unique_ptr<Designator> &&designator) :
            ident_(std::move(designator->ident_)), selectors_(std::move(designator->selectors_)) {};
    Designator(unique_ptr<QualIdent> ident, vector<unique_ptr<Selector>> selectors) :
            ident_(std::move(ident)), selectors_(std::move(selectors)) {};
    virtual ~Designator();

    [[nodiscard]] QualIdent *ident() const;
    [[nodiscard]] vector<unique_ptr<Selector>> &selectors();

    void addSelector(unique_ptr<Selector> selector);
    void insertSelector(size_t num, unique_ptr<Selector> selector);
    void setSelector(size_t num, unique_ptr<Selector> selector);
    void removeSelector(size_t num);
    [[nodiscard]] Selector *getSelector(size_t num) const;
    [[nodiscard]] size_t getSelectorCount() const;

};

#endif //OBERON_LANG_DESIGNATOR_H
