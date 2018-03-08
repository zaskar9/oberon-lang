/*
 * Header file of the AST variable reference nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/6/18.
 */

#ifndef OBERON0C_VARIABLEREFERENCE_H
#define OBERON0C_VARIABLEREFERENCE_H


#include "ExpressionNode.h"
#include "NamedValueNode.h"

class ReferenceNode final : public ExpressionNode {

private:
    const NamedValueNode* node_;

public:
    explicit ReferenceNode(FilePos pos, const NamedValueNode* node);
    ~ReferenceNode() final;

    const NamedValueNode* dereference() const;

    bool isConstant() const final;
    ExpressionType checkType() const final;

    void print(std::ostream &stream) const final;

};


#endif //OBERON0C_VARIABLEREFERENCE_H
