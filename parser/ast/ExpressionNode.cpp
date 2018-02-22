/*
 * Implementation of the AST expression nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/15/18.
 */

#include "ExpressionNode.h"

ExpressionNode::ExpressionNode(const NodeType type, const FilePos pos) : ASTNode(type, pos) {
}

ExpressionNode::~ExpressionNode() = default;

std::ostream& operator<<(std::ostream &stream, const OperatorType &op) {
    std::string result;
    switch (op) {
        case OperatorType::EQ: result = "="; break;
        case OperatorType::NEQ: result = "#"; break;
        case OperatorType::LT: result = "<"; break;
        case OperatorType::GT: result = ">"; break;
        case OperatorType::GEQ: result = ">="; break;
        case OperatorType::LEQ: result = "<="; break;
        case OperatorType::TIMES: result = "*"; break;
        case OperatorType::DIV: result = "DIV"; break;
        case OperatorType::MOD: result = "MOD"; break;
        case OperatorType::PLUS: result = "+"; break;
        case OperatorType::MINUS: result = "-"; break;
        case OperatorType::AND: result = "&"; break;
        case OperatorType::OR: result = "OR"; break;
        case OperatorType::NOT: result = "~"; break;
        case OperatorType::NEG: result = "-"; break;
    }
    stream << result;
    return stream;
}
