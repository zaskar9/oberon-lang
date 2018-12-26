/*
 * Header file of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include <iostream>
#include "Parser.h"
#include "../scanner/IdentToken.h"
#include "../scanner/NumberToken.h"
#include "../scanner/StringToken.h"
#include "ast/UnaryExpressionNode.h"
#include "ast/BinaryExpressionNode.h"
#include "ast/BooleanNode.h"
#include "ast/NumberNode.h"
#include "ast/StringNode.h"
#include "ast/ParameterNode.h"
#include "ast/NamedValueReferenceNode.h"
#include "ast/TypeReferenceNode.h"

Parser::Parser(Scanner *scanner, Logger *logger) :
        scanner_(scanner), logger_(logger) {
    symbols_ = std::make_unique<SymbolTable>();
}

Parser::~Parser() = default;

std::unique_ptr<Node> Parser::parse() {
    return module();
}

const std::string Parser::ident() {
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::const_ident) {
        auto ident = dynamic_cast<const IdentToken*>(token.get());
        logger_->debug("", to_string(*ident));
        return ident->getValue();
    } else {
        logger_->error(token->getPosition(), "identifier expected.");
    }
    return "";
}

// module = "MODULE" identifier ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
std::unique_ptr<ModuleNode> Parser::module() {
    logger_->debug("", "module");
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_module) {
        auto module = std::make_unique<ModuleNode>(token->getPosition(), ident());
        token = scanner_->nextToken();
        if (token->getType() == TokenType::semicolon) {
            declarations(module.get());
            token = scanner_->nextToken();
            if (token->getType() == TokenType::kw_begin) {
                statement_sequence();
                token = scanner_->nextToken();
            }
            if (token->getType() == TokenType::kw_end) {
                std::string name = ident();
                if (name == module->getName()) {
                    token = scanner_->nextToken();
                    if (token->getType() == TokenType::period) {
                        return module;
                    } else {
                        logger_->error(token->getPosition(), ". expected.");
                    }
                } else {
                    logger_->error(token->getPosition(), module->getName() + " expected.");
                }
            } else {
                logger_->error(token->getPosition(), "END expected.");
            }
        } else {
            logger_->error(token->getPosition(), "; expected.");
        }
    } else {
        logger_->error(token->getPosition(), "MODULE expected.");
    }
    return nullptr;
}

// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { ProcedureDeclaration } .
void Parser::declarations(BlockNode *block) {
    logger_->debug("", "declarations");
    if (scanner_->peekToken()->getType() == TokenType::kw_const) {
        const_declarations(block);
    }
    if (scanner_->peekToken()->getType() == TokenType::kw_type) {
        type_declarations(block);
    }
    if (scanner_->peekToken()->getType() == TokenType::kw_var) {
        var_declarations(block);
    }
    while (scanner_->peekToken()->getType() == TokenType::kw_procedure) {
        procedure_declaration(block);
    }
}

// const_declarations = "CONST" { identifier "=" expression ";" } .
void Parser::const_declarations(BlockNode *block) {
    logger_->debug("", "const_declarations");
    scanner_->nextToken(); // skip CONST keyword
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
        const std::string name = ident();
        if (symbols_->isDuplicate(name)) {
            logger_->error(scanner_->peekToken()->getPosition(), "duplicate definition: " + name);
        }
        auto token = scanner_->nextToken();
        if (token->getType() == TokenType::op_eq) {
            auto expr = expression();
            if (expr->isConstant()) {
                auto value = fold(expr.get());
                auto constant = std::make_unique<ConstantNode>(token->getPosition(), name, std::move(value));
                symbols_->insert(name, constant.get());
                block->addConstant(std::move(constant));
            } else {
                logger_->error(token->getPosition(), "expression must be constant.");
            }
            token = scanner_->nextToken();
            if (token->getType() != TokenType::semicolon) {
                logger_->error(token->getPosition(), "; expected.");
            }
        } else {
            logger_->error(token->getPosition(), "= expected.");
        }
    }
}

// type_declarations =  "TYPE" { identifier "=" type ";" } .
void Parser::type_declarations(BlockNode *block) {
    logger_->debug("", "type_declarations");
    scanner_->nextToken(); // skip TYPE keyword
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
        std::string name = ident();
        if (symbols_->isDuplicate(name)) {
            logger_->error(scanner_->peekToken()->getPosition(), "duplicate definition: " + name);
        }
        auto token = scanner_->nextToken();
        if (token->getType() == TokenType::op_eq) {
            auto node = type();
            symbols_->insert(name, node.get());
            block->addType(std::move(node));
            token = scanner_->nextToken();
            if (token->getType() != TokenType::semicolon) {
                logger_->error(token->getPosition(), "; expected.");
            }
        } else {
            logger_->error(token->getPosition(), "= expected.");
        }
    }
}

// var_declarations =  "VAR" { ident_list ":" type ";" } .
void Parser::var_declarations(BlockNode *block) {
    logger_->debug("", "var_declarations");
    scanner_->nextToken(); // skip VAR keyword
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
        std::vector<std::string> idents;
        ident_list(idents);
        auto token = scanner_->nextToken();
        auto pos = token->getPosition();
        if (token->getType() == TokenType::colon) {
            auto node = type();
            block->addType(std::move(node));
            for (auto&& ident : idents) {
                if (symbols_->isDuplicate(ident)) {
                    logger_->error(token->getPosition(), "duplicate definition: " + ident);
                    continue;
                }
                auto variable = std::make_unique<VariableNode>(pos, ident, node.get());
                symbols_->insert(ident, variable.get());
                block->addVariable(std::move(variable));
            }
            token = scanner_->nextToken();
            if (token->getType() != TokenType::semicolon) {
                logger_->error(token->getPosition(), "; expected.");
            }
        } else {
            logger_->error(token->getPosition(), ": expected.");
        }
    }
}

// procedure_declaration = procedure_heading ";" procedure_body identifier ";" .
void Parser::procedure_declaration(BlockNode *block) {
    logger_->debug("", "procedure_declaration");
    symbols_->enterScope();
    auto proc = procedure_heading();
    auto token = scanner_->nextToken();
    if (token->getType() != TokenType::semicolon) {
        logger_->error(token->getPosition(), "; expected.");
    }
    procedure_body(proc.get());
    ident();
    token = scanner_->nextToken();
    if (token->getType() != TokenType::semicolon) {
        logger_->error(token->getPosition(),"; expected.");
    }
    symbols_->leaveScope();
    symbols_->insert(proc->getName(), proc.get());
    block->addProcedure(std::move(proc));
}

// expression = simple_expression [ ( "=" | "#" | "<" | "<=" | ">" | ">=" ) simple_expression ] .
std::unique_ptr<ExpressionNode> Parser::expression() {
    logger_->debug("", "expression");
    auto lhs = simple_expression();
    TokenType type = scanner_->peekToken()->getType();
    if (type == TokenType::op_eq
        || type == TokenType::op_neq
        || type == TokenType::op_lt
        || type == TokenType::op_leq
        || type == TokenType::op_gt
        || type == TokenType::op_geq) {
            auto token = scanner_->nextToken();
            OperatorType op = token_to_operator(token->getType());
            auto rhs = simple_expression();
            return std::make_unique<BinaryExpressionNode>(token->getPosition(), op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

// simple_expression = [ "+" | "-" ] term { ( "+" | "-" | OR ) term } .
std::unique_ptr<ExpressionNode> Parser::simple_expression() {
    logger_->debug("", "simple_expression");
    std::unique_ptr<ExpressionNode> expr;
    TokenType type = scanner_->peekToken()->getType();
    if (type == TokenType::op_plus) {
        scanner_->nextToken();
        expr = term();
    } else if (type == TokenType::op_minus) {
        auto token = scanner_->nextToken();
        expr = std::make_unique<UnaryExpressionNode>(token->getPosition(), OperatorType::NEG, term());
    } else {
        expr = term();
    }
    type = scanner_->peekToken()->getType();
    while (type == TokenType::op_plus
           || type == TokenType::op_minus
           || type == TokenType::op_or) {
        auto token = scanner_->nextToken();
        OperatorType op = token_to_operator(token->getType());
        expr = std::make_unique<BinaryExpressionNode>(token->getPosition(), op, std::move(expr), term());
        type = scanner_->peekToken()->getType();
    }
    return expr;
}

// term = factor { ( "*" | "DIV" | "MOD" | "&" ) factor } .
std::unique_ptr<ExpressionNode> Parser::term() {
    logger_->debug("", "term");
    auto expr = factor();
    TokenType type = scanner_->peekToken()->getType();
    while (type == TokenType::op_times
           || type == TokenType::op_div
           || type == TokenType::op_mod
           || type == TokenType::op_and) {
        auto token = scanner_->nextToken();
        OperatorType op = token_to_operator(token->getType());
        expr = std::make_unique<BinaryExpressionNode>(token->getPosition(), op, std::move(expr), factor());
        type = scanner_->peekToken()->getType();
    }
    return expr;
}

// factor = identifier { selector } | number | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
std::unique_ptr<ExpressionNode> Parser::factor() {
    logger_->debug("", "factor");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::const_ident) {
        FilePos pos = token->getPosition();
        std::string name = ident();
        auto node = symbols_->lookup(name);
        if (node != nullptr) {
            token = scanner_->peekToken();
            if (token->getType() == TokenType::period || token->getType() == TokenType::lbrack) {
                selector();
            }
            if (node->getNodeType() == NodeType::constant ||
                node->getNodeType() == NodeType::parameter ||
                node->getNodeType() == NodeType::variable) {
                return std::make_unique<NamedValueReferenceNode>(pos, dynamic_cast<const NamedValueNode*>(node));
            } else {
                logger_->error(token->getPosition(), "constant, parameter or variable expected.");
                return nullptr;
            }
        } else {
            logger_->error(token->getPosition(), "undefined identifier: " + name + ".");
            return nullptr;
        }
    } else if (token->getType() == TokenType::const_number) {
        auto number = dynamic_cast<const NumberToken*>(scanner_->nextToken().get());
        return std::make_unique<NumberNode>(number->getPosition(), number->getValue());
    } else if (token->getType() == TokenType::const_string) {
        auto string = dynamic_cast<const StringToken*>(scanner_->nextToken().get());
        return std::make_unique<StringNode>(string->getPosition(), string->getValue());
    } else if (token->getType() == TokenType::const_true) {
        scanner_->nextToken();
        return std::make_unique<BooleanNode>(token->getPosition(), true);
    } else if (token->getType() == TokenType::const_false) {
        scanner_->nextToken();
        return std::make_unique<BooleanNode>(token->getPosition(), false);
    } else if (token->getType() == TokenType::lparen) {
        scanner_->nextToken();
        auto expr = expression();
        token = scanner_->peekToken();
        if (token->getType() == TokenType::rparen) {
            scanner_->nextToken();
        } else {
            logger_->error(token->getPosition(), ") expected.");
        }
        return expr;
    } else if (token->getType() == TokenType::op_not) {
        scanner_->nextToken();
        return std::make_unique<UnaryExpressionNode>(token->getPosition(), OperatorType::NOT, factor());
    } else {
        logger_->error(token->getPosition(), "unexpected token: " + to_string(*token));
        return nullptr;
    }
}

// type = identifier | array_type | record_type .
std::unique_ptr<TypeNode> Parser::type() {
    logger_->debug("", "type");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::const_ident) {
        std::string name = ident();
        auto node = symbols_->lookup(name);
        if (node == nullptr) {
            logger_->error(token->getPosition(), "undefined type: " + name + ".");
        } else if (node->getNodeType() == NodeType::array_type ||
                   node->getNodeType() == NodeType::basic_type ||
                   node->getNodeType() == NodeType::record_type) {
            return std::make_unique<TypeReferenceNode>(token->getPosition(), dynamic_cast<const TypeNode*>(node));
        } else {
            logger_->error(token->getPosition(), name + " is not a type.");
        }
    } else if (token->getType() == TokenType::kw_array) {
        return array_type();
    } else if (token->getType() == TokenType::kw_record) {
        return record_type();
    } else {
        logger_->error(token->getPosition(), "unexpected token.");
    }
    return nullptr;
}

// array_type = "ARRAY" expression "OF" type .
std::unique_ptr<ArrayTypeNode> Parser::array_type() {
    logger_->debug("", "array_type");
    FilePos pos = scanner_->nextToken()->getPosition(); // skip ARRAY keyword and get its position
    auto expr = expression();
    if (expr->isConstant() && expr->getType() == BasicTypeNode::INTEGER) {
        int dim = foldNumber(expr.get());
        auto token = scanner_->nextToken();
        if (token->getType() == TokenType::kw_of) {
            return std::make_unique<ArrayTypeNode>(pos, dim, type());
        } else {
            logger_->error(token->getPosition(), "OF expected.");
        }
    }
    return nullptr;
}

// record_type = "RECORD" field_list { ";" field_list } "END" .
std::unique_ptr<RecordTypeNode> Parser::record_type() {
    logger_->debug("", "record_type");
    FilePos pos = scanner_->nextToken()->getPosition(); // skip RECORD keyword and get its position
    auto node = std::make_unique<RecordTypeNode>(pos);
    field_list(node.get());
    while (scanner_->peekToken()->getType() == TokenType::semicolon) {
        field_list(node.get());
    }
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::kw_end) {
        scanner_->nextToken();
    } else {
        logger_->error(token->getPosition(), "END expected.");
    }
    return node;
}

// field_list = ident_list ":" type .
void Parser::field_list(RecordTypeNode *rtype) {
    logger_->debug("", "field_list");
    std::vector<std::string> idents;
    ident_list(idents);
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::colon) {
        auto node = type();
        rtype->addType(std::move(node));
        for (int i = 0; i < idents.size(); i++) {
            rtype->addField(std::make_unique<FieldNode>(token->getPosition(), idents[i], node.get()));
        }
    } else {
        logger_->error(token->getPosition(), ": expected.");
    }
}

// ident_list = ident { "," identifier } .
void Parser::ident_list(std::vector<std::string> &idents) {
    logger_->debug("", "ident_list");
    idents.push_back(ident());
    while (scanner_->peekToken()->getType() == TokenType::comma) {
        scanner_->nextToken(); // skip comma
        idents.push_back(ident());
    }
}

// procedure_heading = "PROCEDURE" identifier [ formal_parameters ] .
std::unique_ptr<ProcedureNode> Parser::procedure_heading() {
    logger_->debug("", "procedure_heading");
    auto token = scanner_->nextToken(); // skip PROCEDURE keyword
    auto proc = std::make_unique<ProcedureNode>(token->getPosition(), ident());
    if (scanner_->peekToken()->getType() == TokenType::lparen) {
        formal_parameters(proc.get());
    }
    return proc;
}

// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
void Parser::procedure_body(ProcedureNode *proc) {
    logger_->debug("", "procedure_body");
    declarations(proc);
    if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
        scanner_->nextToken(); // skip BEGIN keyword
        statement_sequence();
    }
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::kw_end) {
        scanner_->nextToken();
    } else {
        logger_->error(token->getPosition(), "END expected.");
    }
}

// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")".
void Parser::formal_parameters(ProcedureNode *proc) {
    logger_->debug("", "formal_parameters");
    auto token = scanner_->nextToken(); // skip left parenthesis
    if (token->getType() == TokenType::lparen) {
        TokenType type = scanner_->peekToken()->getType();
        if (type == TokenType::kw_var || type == TokenType::const_ident) {
            fp_section(proc);
            while (scanner_->peekToken()->getType() == TokenType::semicolon) {
                scanner_->nextToken(); // skip semicolon
                fp_section(proc);
            }
        }
        token = scanner_->nextToken();
        if (token->getType() != TokenType::rparen) {
            logger_->error(token->getPosition(), ") expected.");
        }
    }
}

// fp_section = [ "VAR" ] ident_list ":" type .
void Parser::fp_section(ProcedureNode *proc) {
    logger_->debug("", "fp_section");
    bool var = false;
    if (scanner_->peekToken()->getType() == TokenType::kw_var) {
        scanner_->nextToken(); // skip VAR keyword
        var = true;
    }
    std::vector<std::string> idents;
    ident_list(idents);
    auto token = scanner_->nextToken();
    if (token->getType() != TokenType::colon) {
        logger_->error(token->getPosition(), ": expected.");
    }
    auto node = type();
    proc->addType(std::move(node));
    for (int i = 0; i < idents.size(); i++) {
        auto param = std::make_unique<ParameterNode>(token->getPosition(), idents[i], node.get(), var);
        symbols_->insert(param->getName(), param.get());
        proc->addParameter(std::move(param));
    }
}

// statement_sequence = statement { ";" statement } .
const Node* Parser::statement_sequence() {
    logger_->debug("", "statement_sequence");
    statement();
    while (scanner_->peekToken()->getType() == TokenType::semicolon) {
        scanner_->nextToken(); // skip semicolon
        statement();
    }
    return nullptr;
}

// statement = [ assignment | procedure_call | if_statement | while_statement ] .
const Node* Parser::statement() {
    logger_->debug("", "statement");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::const_ident) {
        const std::string name = ident();
        auto symbol = symbols_->lookup(name);
        if (symbol == nullptr) {
            logger_->error(token->getPosition(), "undefined identifier: " + name + ".");
        }
        token = scanner_->peekToken();
        if (token->getType() == TokenType::period
            || token->getType() == TokenType::lbrack) {
            selector();
        }
        token = scanner_->peekToken();
        if (token->getType() == TokenType::op_becomes) {
            assignment();
        } else {
            procedure_call();
        }
    } else if (token->getType() == TokenType::kw_if) {
        if_statement();
    } else if (token->getType() == TokenType::kw_while) {
        while_statement();
    } else {
        logger_->error(token->getPosition(), "unknown statement.");
    }
    return nullptr;
}

// assignment = identifier selector ":=" expression .
const Node* Parser::assignment() {
    logger_->debug("", "assignment");
    scanner_->nextToken(); // skip becomes
    expression();
    return nullptr;
}

// procedure_call = identifier [ actual_parameters ] .
const Node* Parser::procedure_call() {
    logger_->debug("", "procedure_call");
    if (scanner_->peekToken()->getType() == TokenType::lparen) {
        actual_parameters();
    }
    return nullptr;
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
const Node* Parser::if_statement() {
    logger_->debug("", "if_statement");
    scanner_->nextToken(); // skip IF keyword
    expression();
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_then) {
        statement_sequence();
        TokenType type = scanner_->peekToken()->getType();
        while (type == TokenType::kw_elsif) {
            scanner_->nextToken(); // skip ELSIF keyword
            statement_sequence();
            type = scanner_->peekToken()->getType();
        }
        if (type == TokenType::kw_else) {
            statement_sequence();
        }
        token = scanner_->nextToken();
        if (token->getType() != TokenType::kw_end) {
            logger_->error(token->getPosition(), "END expected.");
        }
    } else {
        logger_->error(token->getPosition(), "THEN expected, instead of " + to_string(token->getType()) + ".");
    }
    return nullptr;
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
const Node* Parser::while_statement() {
    logger_->debug("", "while_statement");
    scanner_->nextToken(); // skip WHILE keyword
    expression();
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_do) {
        statement_sequence();
        token = scanner_->nextToken();
        if (token->getType() != TokenType::kw_end) {
            logger_->error(token->getPosition(), "END expected.");
        }
    } else {
        logger_->error(token->getPosition(), "DO expected.");
    }
    return nullptr;
}

// actual_parameters = "(" [ expression { "," expression } ] ")" .
const Node* Parser::actual_parameters() {
    logger_->debug("", "actual_parameters");
    scanner_->nextToken(); // skip left parenthesis
    if (scanner_->peekToken()->getType() == TokenType::rparen) {
        scanner_->nextToken();
        return nullptr;
    }
    expression();
    while (scanner_->peekToken()->getType() == TokenType::comma) {
        scanner_->nextToken(); // skip comma
        expression();
    }
    auto token = scanner_->nextToken();
    if (token->getType() != TokenType::rparen) {
        logger_->error(token->getPosition(), ") expected.");
    }
    return nullptr;
}

// selector = {"." identifier | "[" expression "]"}.
const Node* Parser::selector() {
    logger_->debug("", "selector");
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::period) {
        ident();
    } else if (token->getType() == TokenType::lbrack) {
        expression();
        token = scanner_->nextToken();
        if (token->getType() != TokenType::rbrack) {
            logger_->error(token->getPosition(), "] expected.");
        }
    }
    return nullptr;
}

std::unique_ptr<ValueNode> Parser::fold(const ExpressionNode *expr) const {
    auto type = expr->getType();
    if (type == BasicTypeNode::INTEGER) {
        return std::make_unique<NumberNode>(expr->getFilePos(), foldNumber(expr));
    } else if (type == BasicTypeNode::BOOLEAN) {
        return std::make_unique<BooleanNode>(expr->getFilePos(), foldBoolean(expr));
    } else if (type == BasicTypeNode::STRING) {
        return std::make_unique<StringNode>(expr->getFilePos(), foldString(expr));
    } else {
        logger_->error(expr->getFilePos(), "incompatible types.");
        return nullptr;
    }
}

const int Parser::foldNumber(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode*>(expr);
        int value = foldNumber(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NEG: return -1 * value;
            case OperatorType::PLUS: return value;
            default:
                logger_->error(unExpr->getFilePos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode*>(expr);
        int lValue = foldNumber(binExpr->getLeftExpression());
        int rValue = foldNumber(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:  return lValue + rValue;
            case OperatorType::MINUS: return lValue - rValue;
            case OperatorType::TIMES: return lValue * rValue;
            case OperatorType::DIV:   return lValue / rValue;
            case OperatorType::MOD:   return lValue % rValue;
            default:
                logger_->error(binExpr->getFilePos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::number) {
        auto numConst = dynamic_cast<const NumberNode *>(expr);
        return numConst->getValue();
    } else if (expr->getNodeType() == NodeType::name_reference) {
        auto ref = dynamic_cast<const NamedValueReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantNode*>(ref->dereference());
            return foldNumber(constant->getValue());
        }
    } else {
        logger_->error(expr->getFilePos(), "incompatible expression.");
    }
    return 0;
}

const bool Parser::foldBoolean(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::unary_expression) {
        auto unExpr = dynamic_cast<const UnaryExpressionNode*>(expr);
        bool value = foldBoolean(unExpr->getExpression());
        switch (unExpr->getOperator()) {
            case OperatorType::NOT: return !value;
            default:
                logger_->error(unExpr->getFilePos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode*>(expr);
        OperatorType op = binExpr->getOperator();
        auto lhs = binExpr->getLeftExpression();
        auto rhs = binExpr->getRightExpression();
        auto type = lhs->getType();
        if (type == BasicTypeNode::BOOLEAN) {
            bool lValue = foldBoolean(binExpr->getLeftExpression());
            bool rValue = foldBoolean(binExpr->getRightExpression());
            switch (op) {
                case OperatorType::AND: return lValue && rValue;
                case OperatorType::OR:  return lValue || rValue;
                default:
                    logger_->error(binExpr->getFilePos(), "incompatible operator.");
            }
        } else if (type == BasicTypeNode::INTEGER) {
            int lValue = foldNumber(lhs);
            int rValue = foldNumber(rhs);
            switch (op) {
                case OperatorType::EQ:  return lValue == rValue;
                case OperatorType::NEQ: return lValue != rValue;
                case OperatorType::LT:  return lValue < rValue;
                case OperatorType::LEQ: return lValue <= rValue;
                case OperatorType::GT:  return lValue > rValue;
                case OperatorType::GEQ: return lValue >= rValue;
                default:
                    logger_->error(binExpr->getFilePos(), "incompatible operator.");
            }
        } else if (type == BasicTypeNode::STRING) {
            std::string lValue = foldString(lhs);
            std::string rValue = foldString(rhs);
            switch (op) {
                case OperatorType::EQ:  return lValue == rValue;
                case OperatorType::NEQ: return lValue != rValue;
                default:
                    logger_->error(binExpr->getFilePos(), "incompatible operator.");
            }
        } else {
            logger_->error(expr->getFilePos(), "incompatible expression.");
        }
    } else if (expr->getNodeType() == NodeType::boolean) {
        auto boolConst = dynamic_cast<const BooleanNode*>(expr);
        return boolConst->getValue();
    } else if (expr->getNodeType() == NodeType::name_reference) {
        auto ref = dynamic_cast<const NamedValueReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantNode*>(ref->dereference());
            foldBoolean(constant->getValue());
        }
    } else {
        logger_->error(expr->getFilePos(), "incompatible expression.");
    }
    return false;
}

const std::string Parser::foldString(const ExpressionNode *expr) const {
    if (expr->getNodeType() == NodeType::binary_expression) {
        auto binExpr = dynamic_cast<const BinaryExpressionNode*>(expr);
        std::string lValue = foldString(binExpr->getLeftExpression());
        std::string rValue = foldString(binExpr->getRightExpression());
        switch (binExpr->getOperator()) {
            case OperatorType::PLUS:
                return std::string(lValue + rValue);
            default:
                logger_->error(binExpr->getFilePos(), "incompatible operator.");
        }
    } else if (expr->getNodeType() == NodeType::string) {
        auto stringConst = dynamic_cast<const StringNode *>(expr);
        return stringConst->getValue();
    } else if (expr->getNodeType() == NodeType::constant) {
        auto ref = dynamic_cast<const NamedValueReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantNode*>(ref->dereference());
            foldString(constant->getValue());

        }
    } else {
        logger_->error(expr->getFilePos(), "incompatible expression.");
    }
    return "";
}

OperatorType token_to_operator(TokenType token) {
    switch(token) {
        case TokenType::op_eq:    return OperatorType::EQ;
        case TokenType::op_neq:   return OperatorType::NEQ;
        case TokenType::op_leq:   return OperatorType::LEQ;
        case TokenType::op_geq:   return OperatorType::GEQ;
        case TokenType::op_lt:    return OperatorType::LT;
        case TokenType::op_gt:    return OperatorType::GT;
        case TokenType::op_times: return OperatorType::TIMES;
        case TokenType::op_div:   return OperatorType::DIV;
        case TokenType::op_mod:   return OperatorType::MOD;
        case TokenType::op_plus:  return OperatorType::PLUS;
        case TokenType::op_minus: return OperatorType::MINUS;
        case TokenType::op_and:   return OperatorType::AND;
        case TokenType::op_or:    return OperatorType::OR;
        case TokenType::op_not:   return OperatorType::NOT;
        default:
            exit(1);
    }
}
