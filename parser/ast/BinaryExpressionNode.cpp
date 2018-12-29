/*
 * Implementation of the AST binary expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#include "BinaryExpressionNode.h"
#include "BasicTypeNode.h"
#include "TypeReferenceNode.h"

BinaryExpressionNode::BinaryExpressionNode(const FilePos pos, const OperatorType op,
                                           std::unique_ptr<ExpressionNode> lhs,
                                           std::unique_ptr<ExpressionNode> rhs) :
        ExpressionNode(NodeType::binary_expression, pos), op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {
}

BinaryExpressionNode::~BinaryExpressionNode() = default;

bool BinaryExpressionNode::isConstant() const {
    return lhs_->isConstant() && rhs_->isConstant();
}

const TypeNode* BinaryExpressionNode::getType() const {
    auto lhsType = lhs_->getType();
    auto rhsType = rhs_->getType();
    if (lhsType == rhsType) {
        if (op_ == OperatorType::EQ
            || op_ == OperatorType::NEQ
            || op_ == OperatorType::LT
            || op_ == OperatorType::LEQ
            || op_ == OperatorType::GT
            || op_ == OperatorType::GEQ) {
            return BasicTypeNode::BOOLEAN;
        }
        return lhsType;
    }
    std::cerr << lhsType << ", " << rhsType << std::endl;
    return nullptr;
}

OperatorType BinaryExpressionNode::getOperator() const {
    return op_;
}

const ExpressionNode* BinaryExpressionNode::getLeftExpression() const {
    return lhs_.get();
}

const ExpressionNode* BinaryExpressionNode::getRightExpression() const {
    return rhs_.get();
}

void BinaryExpressionNode::print(std::ostream &stream) const {
    lhs_->print(stream);
    stream << " ";
    stream << op_;
    stream << " ";
    rhs_->print(stream);
}

