/*
 * Header file of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include <iostream>
#include "Parser.h"
#include "ast/BinaryExpressionNode.h"
#include "ast/BooleanConstantNode.h"
#include "ast/NumberConstantNode.h"
#include "ast/StringConstantNode.h"
#include "ast/UnaryExpressionNode.h"
#include "symbol/NumberConstantSymbol.h"
#include "symbol/BooleanConstantSymbol.h"
#include "symbol/StringConstantSymbol.h"

Parser::Parser(Scanner *scanner, Table *symbols, Logger *logger) :
        scanner_(scanner), symbols_(symbols), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const ASTNode> Parser::parse() {
    return std::make_unique<const ASTNode>(module());
}

// module = "MODULE" identifier ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
const ASTNode* Parser::module() {
    logger_->debug("", "module");
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_module) {
        ident();
        token = scanner_->nextToken();
        if (token->getType() == TokenType::semicolon) {
            declarations();
            token = scanner_->nextToken();
            if (token->getType() == TokenType::kw_begin) {
                statement_sequence();
                token = scanner_->nextToken();
            }
            if (token->getType() == TokenType::kw_end) {
                ident();
                token = scanner_->nextToken();
                if (token->getType() != TokenType::period) {
                    logger_->error(token->getPosition(), ". expected.");
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

const std::string Parser::ident() {
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::const_ident) {
        auto ident = dynamic_pointer_cast<const IdentToken>(token);
        logger_->debug("", "identifier : " + ident.getValue());
        return ident;
    } else {
        logger_->error(token->getPosition(), "identifier expected.");
    }
    return "";
}

// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { ProcedureDeclaration } .
const ASTNode* Parser::declarations() {
    logger_->debug("", "declarations");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::kw_const) {
        const_declarations();
    }
    token = scanner_->peekToken();
    if (token->getType() == TokenType::kw_type) {
        type_declarations();
    }
    token = scanner_->peekToken();
    if (token->getType() == TokenType::kw_var) {
        var_declarations();
    }
    token = scanner_->peekToken();
    while (token->getType() == TokenType::kw_procedure) {
        procedure_declaration();
        token = scanner_->peekToken();
    }
    return nullptr;
}

// const_declarations = "CONST" { identifier "=" expression ";" } .
const ASTNode* Parser::const_declarations() {
    logger_->debug("", "const_declarations");
    scanner_->nextToken(); // skip CONST keyword
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
        const std::string name = this->ident();
        auto token = scanner_->nextToken();
        if (token->getType() == TokenType::op_eq) {
            auto expr = expression();
            if (expr->isConstant()) {
                auto symbol = fold(name, expr);
                symbols_->insert(symbol);
            } else {
                logger_->error(token.pos, "expression must be constant.");
            }
            token = scanner_->nextToken();
            if (token->getType() != TokenType::semicolon) {
                logger_->error(token.pos, "; expected.");
            }
        } else {
            logger_->error(token.pos, "= expected.");
        }
    }
    return nullptr;
}

// type_declarations =  "TYPE" { identifier "=" type ";" } .
void Parser::type_declarations() {
    logger_->debug("", "type_declarations");
    scanner_->nextToken(); // skip TYPE keyword
    Token token = scanner_->peekToken();
    while (token.type == TokenType::const_ident) {
        std::string name = ident();
        token = scanner_->nextToken();
        if (token.type == TokenType::op_eq) {
            std::shared_ptr<const TypeSymbol> ts = type();
            symbols_->insert(name, ts);
            token = scanner_->nextToken();
            if (token.type != TokenType::semicolon) {
                logger_->error(token.pos, "; expected.");
            }
        } else {
            logger_->error(token.pos, "= expected.");
        }
        token = scanner_->peekToken();
    }
}

// var_declarations =  "VAR" { ident_list ":" type ";" } .
const ASTNode* Parser::var_declarations() {
    logger_->debug("", "var_declarations");
    scanner_->nextToken(); // skip VAR keyword
    Token token = scanner_->peekToken();
    while (token.type == TokenType::const_ident) {
        std::list<std::string> idents;
        ident_list(idents);
        token = scanner_->nextToken();
        if (token.type == TokenType::colon) {
            type();
            token = scanner_->nextToken();
            if (token.type != TokenType::semicolon) {
                logger_->error(token.pos, "; expected.");
            }
        } else {
            logger_->error(token.pos, ": expected.");
        }
        token = scanner_->peekToken();
    }
    return nullptr;
}

// procedure_declaration = procedure_heading ";" procedure_body identifier ";" .
const ASTNode* Parser::procedure_declaration() {
    logger_->debug("", "procedure_declaration");
    procedure_heading();
    Token token = scanner_->nextToken();
    if (token.type != TokenType::semicolon) {
        logger_->error(token.pos, "; semicolon.");
    }
    procedure_body();
    ident();
    token = scanner_->nextToken();
    if (token.type != TokenType::semicolon) {
        logger_->error(token.pos,"; semicolon.");
    }
    return nullptr;
}

// expression = simple_expression [ ( "=" | "#" | "<" | "<=" | ">" | ">=" ) simple_expression ] .
const std::shared_ptr<const ExpressionNode> Parser::expression() {
    logger_->debug("", "expression");
    std::shared_ptr<const ExpressionNode> lhs = simple_expression();
    Token token = scanner_->peekToken();
    if (token.type == TokenType::op_eq
        || token.type == TokenType::op_neq
        || token.type == TokenType::op_lt
        || token.type == TokenType::op_leq
        || token.type == TokenType::op_gt
        || token.type == TokenType::op_geq) {
            token = scanner_->nextToken();
            OperatorType op = token_to_operator(token.type);
            std::shared_ptr<const ExpressionNode> rhs = simple_expression();
            return std::make_shared<BinaryExpressionNode>(token.pos, op, lhs, rhs);
    }
    return lhs;
}

// simple_expression = [ "+" | "-" ] term { ( "+" | "-" | OR ) term } .
const std::shared_ptr<const ExpressionNode> Parser::simple_expression() {
    logger_->debug("", "simple_expression");
    std::shared_ptr<const ExpressionNode> expr;
    Token token = scanner_->peekToken();
    if (token.type == TokenType::op_plus) {
        scanner_->nextToken();
        expr = term();
    } else if (token.type == TokenType::op_minus) {
        scanner_->nextToken();
        expr = std::make_shared<UnaryExpressionNode>(token.pos, OperatorType::NEG, term());
    } else {
        expr = term();
    }
    token = scanner_->peekToken();
    while (token.type == TokenType::op_plus
           || token.type == TokenType::op_minus
           || token.type == TokenType::op_or) {
        token = scanner_->nextToken();
        OperatorType op = token_to_operator(token.type);
        expr = std::make_shared<BinaryExpressionNode>(token.pos, op, expr, term());
        token = scanner_->peekToken();
    }
    return expr;
}

// term = factor { ( "*" | "DIV" | "MOD" | "&" ) factor } .
const std::shared_ptr<const ExpressionNode> Parser::term() {
    logger_->debug("", "term");
    std::shared_ptr<const ExpressionNode> expr = factor();
    Token token = scanner_->peekToken();
    while (token.type == TokenType::op_times
           || token.type == TokenType::op_div
           || token.type == TokenType::op_mod
           || token.type == TokenType::op_and) {
        token = scanner_->nextToken();
        OperatorType op = token_to_operator(token.type);
        expr = std::make_shared<BinaryExpressionNode>(token.pos, op, expr, factor());
        token = scanner_->peekToken();
    }
    return expr;
}

// factor = identifier { selector } | number | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
const std::shared_ptr<const ExpressionNode> Parser::factor() {
    logger_->debug("", "factor");
    Token token = scanner_->peekToken();
    if (token.type == TokenType::const_ident) {
        std::string name = ident();
        auto symbol = symbols_->lookup(name);
        if (symbol == nullptr) {
            logger_->error(token.pos, "undefined identifier: " + name + ".");
        }
        token = scanner_->peekToken();
        if (token.type == TokenType::period
            || token.type == TokenType::lbrack) {
            selector();
        }
    } else if (token.type == TokenType::const_number) {
        scanner_->nextToken();
        return std::make_shared<NumberConstantNode>(token.pos, scanner_->getNumValue());
    } else if (token.type == TokenType::const_string) {
        scanner_->nextToken();
        return std::make_shared<StringConstantNode>(token.pos, scanner_->getStrValue());
    } else if (token.type == TokenType::const_true) {
        scanner_->nextToken();
        return std::make_shared<BooleanConstantNode>(token.pos, true);
    } else if (token.type == TokenType::const_false) {
        scanner_->nextToken();
        return std::make_shared<BooleanConstantNode>(token.pos, false);
    } else if (token.type == TokenType::lparen) {
        scanner_->nextToken();
        std::shared_ptr<const ExpressionNode> expr = expression();
        token = scanner_->nextToken();
        if (token.type != TokenType::rparen) {
            logger_->error(token.pos, ") expected.");
        }
        return expr;
    } else if (token.type == TokenType::op_not) {
        scanner_->nextToken();
        return std::make_shared<UnaryExpressionNode>(token.pos, OperatorType::NOT, factor());
    } else {
        logger_->error(token.pos, "unexpected token.");
    }
    return nullptr;
}

// type = identifier | array_type | record_type .
const std::shared_ptr<const TypeSymbol> Parser::type() {
    logger_->debug("", "type");
    Token token = scanner_->peekToken();
    if (token.type == TokenType::const_ident) {
        std::string name = ident();
        auto symbol = symbols_->lookup(name);
        if (symbol == nullptr) {
            logger_->error(token.pos, "undefined type: " + name + ".");
        } else if (symbol->getSymbolType() == SymbolType::basic_type
                   || symbol->getSymbolType() == SymbolType::array_type
                   || symbol->getSymbolType() == SymbolType::record_type) {
            return std::dynamic_pointer_cast<const TypeSymbol>(symbol);
        } else {
            logger_->error(token.pos, name + " is not a type.");
        }
    } else if (token.type == TokenType::kw_array) {
        return array_type();
    } else if (token.type == TokenType::kw_record) {
        return record_type();
    } else {
        logger_->error(token.pos, "unexpected token.");
    }
    return nullptr;
}

// array_type = "ARRAY" expression "OF" type .
const std::shared_ptr<const ArrayTypeSymbol> Parser::array_type() {
    logger_->debug("", "array_type");
    scanner_->nextToken(); // skip ARRAY keyword
    expression();
    Token token = scanner_->nextToken();
    if (token.type == TokenType::kw_of) {
        std::shared_ptr<const TypeSymbol> ts = type();
        return std::make_shared<ArrayTypeSymbol>(0, ts);
    } else {
        logger_->error(token.pos, "OF expected.");
    }
    return nullptr;
}

// record_type = "RECORD" field_list { ";" field_list } "END" .
const std::shared_ptr<const RecordTypeSymbol> Parser::record_type() {
    logger_->debug("", "record_type");
    scanner_->nextToken(); // skip RECORD keyword
    auto rts = std::make_shared<RecordTypeSymbol>();
    field_list(rts);
    Token token = scanner_->peekToken();
    while (token.type == TokenType::semicolon) {
        field_list(rts);
        token = scanner_->peekToken();
    }
    token = scanner_->nextToken();
    if (token.type != TokenType::kw_end) {
        logger_->error(token.pos, "END expected.");
    }
    return rts;
}

// field_list = ident_list ":" type .
void Parser::field_list(const std::shared_ptr<RecordTypeSymbol> &rts) {
    logger_->debug("", "field_list");
    std::list<std::string> idents;
    ident_list(idents);
    Token token = scanner_->nextToken();
    if (token.type == TokenType::colon) {
        std::shared_ptr<const TypeSymbol> ts = type();
        for (auto const &itr : idents) {
            rts->addField(std::make_shared<VariableSymbol>(itr, ts));
        }
    } else {
        logger_->error(token.pos, ": expected.");
    }
}

// ident_list = ident { "," identifier } .
void Parser::ident_list(std::list<std::string> &idents) {
    logger_->debug("", "ident_list");
    idents.push_back(ident());
    Token token = scanner_->peekToken();
    while (token.type == TokenType::comma) {
        scanner_->nextToken(); // skip comma
        idents.push_back(ident());
        token = scanner_->peekToken();
    }
}

// procedure_heading = "PROCEDURE" identifier [ formal_parameters ] .
const std::shared_ptr<const ProcedureSymbol> Parser::procedure_heading() {
    logger_->debug("", "procedure_heading");
    scanner_->nextToken(); // skip PROCEDURE keyword
    std::shared_ptr<ProcedureSymbol> ps = std::make_shared<ProcedureSymbol>(ident());
    Token token = scanner_->peekToken();
    if (token.type == TokenType::lparen) {
        formal_parameters(ps);
    }
    return nullptr;
}

// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
const ASTNode* Parser::procedure_body() {
    logger_->debug("", "procedure_body");
    declarations();
    Token token = scanner_->peekToken();
    if (token.type == TokenType::kw_begin) {
        scanner_->nextToken(); // skip BEGIN keyword
        statement_sequence();
    }
    token = scanner_->nextToken();
    if (token.type != TokenType::kw_end) {
        logger_->error(token.pos, "END expected.");
    }
    return nullptr;
}

// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")".
void Parser::formal_parameters(const std::shared_ptr<ProcedureSymbol> &ps) {
    logger_->debug("", "formal_parameters");
    Token token = scanner_->nextToken(); // skip left parenthesis
    if (token.type == TokenType::lparen) {
        token = scanner_->peekToken();
        if (token.type == TokenType::kw_var
            || token.type == TokenType::const_ident) {
            fp_section(ps);
            token = scanner_->peekToken();
            while (token.type == TokenType::semicolon) {
                scanner_->nextToken(); // skip semicolon
                fp_section(ps);
                token = scanner_->peekToken();
            }
        }
        token = scanner_->nextToken();
        if (token.type != TokenType::rparen) {
            logger_->error(token.pos, ") expected.");
        }
    }
}

// fp_section = [ "VAR" ] ident_list ":" type .
void Parser::fp_section(const std::shared_ptr<ProcedureSymbol> &ps) {
    logger_->debug("", "fp_section");
    bool var = false;
    Token token = scanner_->peekToken();
    if (token.type == TokenType::kw_var) {
        scanner_->nextToken(); // skip VAR keyword
        var = true;
    }
    std::list<std::string> idents;
    ident_list(idents);
    token = scanner_->nextToken();
    if (token.type != TokenType::colon) {
        logger_->error(token.pos, ": expected.");
    }
    std::shared_ptr<const TypeSymbol> ts = type();
    for (auto const &itr : idents) {
        ps->addParameter(std::make_shared<ParameterSymbol>(itr, ts, var));
    }
}

// statement_sequence = statement { ";" statement } .
const ASTNode* Parser::statement_sequence() {
    logger_->debug("", "statement_sequence");
    statement();
    Token token = scanner_->peekToken();
    while (token.type == TokenType::semicolon) {
        scanner_->nextToken(); // skip semicolon
        statement();
        token = scanner_->peekToken();
    }
    return nullptr;
}

// statement = [ assignment | procedure_call | if_statement | while_statement ] .
const ASTNode* Parser::statement() {
    logger_->debug("", "statement");
    Token token = scanner_->peekToken();
    if (token.type == TokenType::const_ident) {
        const std::string name = ident();
        auto symbol = symbols_->lookup(name);
        if (symbol == nullptr) {
            logger_->error(token.pos, "undefined identifier: " + name + ".");
        }
        token = scanner_->peekToken();
        if (token.type == TokenType::period
            || token.type == TokenType::lbrack) {
            selector();
        }
        token = scanner_->peekToken();
        if (token.type == TokenType::op_becomes) {
            assignment();
        } else {
            procedure_call();
        }
    } else if (token.type == TokenType::kw_if) {
        if_statement();
    } else if (token.type == TokenType::kw_while) {
        while_statement();
    } else {
        logger_->error(token.pos, "unknown statement.");
    }
    return nullptr;
}

// assignment = identifier selector ":=" expression .
const ASTNode* Parser::assignment() {
    logger_->debug("", "assignment");
    scanner_->nextToken(); // skip becomes
    expression();
    return nullptr;
}

// procedure_call = identifier [ actual_parameters ] .
const ASTNode* Parser::procedure_call() {
    logger_->debug("", "procedure_call");
    Token token = scanner_->peekToken();
    if (token.type == TokenType::lparen) {
        actual_parameters();
    }
    return nullptr;
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
const ASTNode* Parser::if_statement() {
    logger_->debug("", "if_statement");
    scanner_->nextToken(); // skip IF keyword
    expression();
    Token token = scanner_->nextToken();
    if (token.type == TokenType::kw_then) {
        statement_sequence();
        token = scanner_->peekToken();
        while (token.type == TokenType::kw_elsif) {
            scanner_->nextToken(); // skip ELSIF keyword
            statement_sequence();
            token = scanner_->peekToken();
        }
        if (token.type == TokenType::kw_else) {
            statement_sequence();
        }
        token = scanner_->nextToken();
        if (token.type != TokenType::kw_end) {
            logger_->error(token.pos, "END expected.");
        }
    } else {
        logger_->error(token.pos, "THEN expected, instead of " + toString(token.type) + ".");
    }
    return nullptr;
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
const ASTNode* Parser::while_statement() {
    logger_->debug("", "while_statement");
    scanner_->nextToken(); // skip WHILE keyword
    expression();
    Token token = scanner_->nextToken();
    if (token.type == TokenType::kw_do) {
        statement_sequence();
        token = scanner_->nextToken();
        if (token.type != TokenType::kw_end) {
            logger_->error(token.pos, "END expected.");
        }
    } else {
        logger_->error(token.pos, "DO expected.");
    }
    return nullptr;
}

// actual_parameters = "(" [ expression { "," expression } ] ")" .
const ASTNode* Parser::actual_parameters() {
    logger_->debug("", "actual_parameters");
    scanner_->nextToken(); // skip left parenthesis
    Token token = scanner_->peekToken();
    if (token.type == TokenType::rparen) {
        scanner_->nextToken();
        return nullptr;
    }
    expression();
    token = scanner_->peekToken();
    while (token.type == TokenType::comma) {
        scanner_->nextToken(); // skip comma
        expression();
        token = scanner_->peekToken();
    }
    token = scanner_->nextToken();
    if (token.type != TokenType::rparen) {
        logger_->error(token.pos, ") expected.");
    }
    return nullptr;
}

// selector = {"." identifier | "[" expression "]"}.
const ASTNode* Parser::selector() {
    logger_->debug("", "selector");
    Token token = scanner_->nextToken();
    if (token.type == TokenType::period) {
        ident();
    } else if (token.type == TokenType::lbrack) {
        expression();
        token = scanner_->nextToken();
        if (token.type != TokenType::rbrack) {
            logger_->error(token.pos, "] expected.");
        }
    }
    return nullptr;
}

const std::unique_ptr<const ConstantSymbol> Parser::fold(const std::string &name, const ExpressionNode *expr) const {
    switch (expr->checkType()) {
        case ExpressionType::INTEGER:
            return std::make_unique<const NumberConstantSymbol>(name, foldNumber(expr));
        case ExpressionType::BOOLEAN:
            return std::make_unique<const BooleanConstantSymbol>(name, foldBoolean(expr));
        case ExpressionType::STRING:
            return std::make_unique<const StringConstantSymbol>(name, foldString(expr));
        case ExpressionType::UNDEF:
            logger_->error(expr->getFilePos(), "incompatible types.");
            return 0;
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
    } else if (expr->getNodeType() == NodeType::number_constant) {
        auto numConst = dynamic_cast<const NumberConstantNode*>(expr);
        return numConst->getValue();
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
        ExpressionType type = lhs->checkType();
        if (type == ExpressionType::BOOLEAN) {
            bool lValue = foldBoolean(binExpr->getLeftExpression());
            bool rValue = foldBoolean(binExpr->getRightExpression());
            switch (op) {
                case OperatorType::AND: return lValue && rValue;
                case OperatorType::OR:  return lValue || rValue;
                default:
                    logger_->error(binExpr->getFilePos(), "incompatible operator.");
            }
        } else if (type == ExpressionType::INTEGER) {
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
        } else if (type == ExpressionType::STRING) {
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
    } else if (expr->getNodeType() == NodeType::boolean_constant) {
        auto boolConst = dynamic_cast<const BooleanConstantNode*>(expr);
        return boolConst->getValue();
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
    } else if (expr->getNodeType() == NodeType::string_constant) {
        auto stringConst = dynamic_cast<const StringConstantNode*>(expr);
        return stringConst->getValue();
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
