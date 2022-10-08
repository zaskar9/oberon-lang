//
// Created by Michael Grossniklaus on 9/29/22.
//

#ifndef OBERON_LANG_SELECTOR_H
#define OBERON_LANG_SELECTOR_H


#include <memory>
#include <string>
#include <vector>
#include "Ident.h"

class Selector {

private:
    NodeType type_;

public:
    explicit Selector(NodeType type) : type_(type) {};
    virtual ~Selector();

    [[nodiscard]] virtual FilePos pos() const = 0;
    [[nodiscard]] NodeType getType() const;
};

class ExpressionNode;

class ArraySelector final : public Selector {

private:
    std::unique_ptr<ExpressionNode> expression_;

public:
    explicit ArraySelector(std::unique_ptr<ExpressionNode> expression);
    ~ArraySelector() override;

    [[nodiscard]] FilePos pos() const override;

    [[nodiscard]] ExpressionNode *getExpression() const;
};


class ValueReferenceNode;

class RecordSelector final : public Selector {

private:
    std::unique_ptr<ValueReferenceNode> ident_;

public:
    explicit RecordSelector(std::unique_ptr<ValueReferenceNode> field);
    ~RecordSelector() override;

    [[nodiscard]] FilePos pos() const override;

    [[nodiscard]] ValueReferenceNode *getField() const;

};


class CaretSelector final : public Selector {

private:
    FilePos pos_;

public:
    explicit CaretSelector(const FilePos &pos) : Selector(NodeType::pointer_type), pos_(pos) {};
    ~CaretSelector() override;

    [[nodiscard]] FilePos pos() const override;

};


#endif //OBERON_LANG_SELECTOR_H
