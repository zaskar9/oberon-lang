/*
 * AST node representing an array type in the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/9/18.
 */

#ifndef OBERON0C_ARRAYTYPESYMBOL_H
#define OBERON0C_ARRAYTYPESYMBOL_H


#include <memory>
#include <utility>
#include "TypeNode.h"
#include "ExpressionNode.h"

class ArrayTypeNode final : public TypeNode {

private:
    std::unique_ptr<ExpressionNode> expr_;
    unsigned int dim_;
    TypeNode *memberType_;

public:
    explicit ArrayTypeNode(const FilePos &pos, std::string name, std::unique_ptr<ExpressionNode> expr, TypeNode* memberType) :
            TypeNode(NodeType::array_type, pos, std::move(name), 0),
            expr_(std::move(expr)), dim_(0), memberType_(memberType) { };
    ~ArrayTypeNode() final = default;

    [[nodiscard]] ExpressionNode * getExpression() const;

    void setDimension(unsigned int dim);
    [[nodiscard]] unsigned int getDimension() const;

    void setMemberType(TypeNode *memberType);
    [[nodiscard]] TypeNode * getMemberType() const;

    void accept(NodeVisitor& visitor) final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_ARRAYTYPESYMBOL_H
