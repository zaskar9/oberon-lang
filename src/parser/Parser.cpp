/*
 * Parser of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include <iostream>
#include "Parser.h"
#include "../scanner/IdentToken.h"
#include "../scanner/LiteralToken.h"
#include "../data/ast/DeclarationNode.h"
#include "../data/ast/NodeReference.h"
#include "../data/ast/IfThenElseNode.h"
#include "../data/ast/LoopNode.h"
#include "../data/ast/AssignmentNode.h"

static OperatorType token_to_operator(TokenType token);

std::unique_ptr<ModuleNode> Parser::parse() {
    return module();
}

std::string Parser::ident() {
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::const_ident)) {
        auto ident = dynamic_cast<const IdentToken*>(token_.get());
        logger_->debug("", to_string(*ident));
        return ident->value();
    }
    return "";
}

// ident_list = ident { "," ident } .
void Parser::ident_list(std::vector<std::string> &idents) {
    logger_->debug("", "ident_list");
    idents.push_back(ident());
    while (scanner_->peek()->type() == TokenType::comma) {
        token_ = scanner_->next(); // skip comma
        idents.push_back(ident());
    }
}

// module = "MODULE" ident ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
std::unique_ptr<ModuleNode> Parser::module() {
    logger_->debug("", "module");
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_module)) {
        auto module = std::make_unique<ModuleNode>(token_->pos(), ident());
        token_ = scanner_->next();
        if (assertToken(token_.get(), TokenType::semicolon)) {
            declarations(module.get());
            token_ = scanner_->next();
            if (token_->type() == TokenType::kw_begin) {
                statement_sequence(module->getStatements());
                token_ = scanner_->next();
            }
            if (assertToken(token_.get(), TokenType::kw_end)) {
                auto name = ident();
                if (name != module->getName()) {
                    logger_->error(scanner_->peek()->pos(),
                                   module->getName() + " expected, found " + name + ".");
                }
                token_ = scanner_->next();
                if (token_->type() != TokenType::period) {
                    logger_->error(token_->pos(), ". expected, found " + to_string(token_->type()) + ".");
                }
            }
        }
        return module;
    }
    return nullptr;
}

// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { procedure_declaration } .
void Parser::declarations(BlockNode *block) {
    logger_->debug("", "declarations");
    if (scanner_->peek()->type() == TokenType::kw_const) {
        const_declarations(block);
    }
    if (scanner_->peek()->type() == TokenType::kw_type) {
        type_declarations(block);
    }
    if (scanner_->peek()->type() == TokenType::kw_var) {
        var_declarations(block);
    }
    while (scanner_->peek()->type() == TokenType::kw_procedure) {
        procedure_declaration(block);
    }
}

// const_declarations = "CONST" { ident "=" expression ";" } .
void Parser::const_declarations(BlockNode *block) {
    logger_->debug("", "const_declarations");
    scanner_->next(); // skip CONST keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        auto pos = scanner_->peek()->pos();
        auto name = ident();
        auto token = scanner_->next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto constant = std::make_unique<ConstantDeclarationNode>(pos, name, expression());
            block->addConstant(std::move(constant));
            token = scanner_->next();
            if (token->type() != TokenType::semicolon) {
                logger_->error(token->pos(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
}

// type_declarations =  "TYPE" { ident "=" type ";" } .
void Parser::type_declarations(BlockNode *block) {
    logger_->debug("", "type_declarations");
    scanner_->next(); // skip TYPE keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        auto pos = scanner_->peek()->pos();
        auto name = ident();
        auto token = scanner_->next();
        if (assertToken(token.get(), TokenType::op_eq)) {
            auto node = std::make_unique<TypeDeclarationNode>(pos, name, type(block, name));
            block->addTypeDeclaration(std::move(node));
            token = scanner_->next();
            if (token->type() != TokenType::semicolon) {
                logger_->error(token->pos(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
}

// type = ( ident | array_type | record_type ) .
TypeNode* Parser::type(BlockNode *block, std::string name) {
    logger_->debug("", "type");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        auto node = std::make_unique<TypeReferenceNode>(token->pos(), ident());
        auto res = node.get();
        block->registerType(std::move(node));
        return res;
    } else if (token->type() == TokenType::kw_array) {
        std::unique_ptr<ArrayTypeNode> node(array_type(block, name));
        auto res = node.get();
        block->registerType(std::move(node));
        return res;
    } else if (token->type() == TokenType::kw_record) {
        std::unique_ptr<RecordTypeNode> node(record_type(block, name));
        auto res = node.get();
        block->registerType(std::move(node));
        return res;
    } else {
        logger_->error(token->pos(), "unexpected token: " + to_string(token->type()) + ".");
        resync({ TokenType::semicolon, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin });
    }
    return nullptr;
}

// array_type = "ARRAY" expression "OF" type .
ArrayTypeNode* Parser::array_type(BlockNode *block, std::string name) {
    logger_->debug("", "array_type");
    FilePos pos = scanner_->next()->pos(); // skip ARRAY keyword and get its position
    auto expr = expression();
    if (assertToken(scanner_->peek(), TokenType::kw_of)) {
        scanner_->next(); // skip OF keyword
        return new ArrayTypeNode(pos, name, std::move(expr), type(block));
    } else {
        resync({TokenType::semicolon, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin});
    }
    return nullptr;
}

// record_type = "RECORD" field_list { ";" field_list } "END" .
RecordTypeNode* Parser::record_type(BlockNode *block, std::string name) {
    logger_->debug("", "record_type");
    FilePos pos = scanner_->next()->pos(); // skip RECORD keyword and get its position
    auto node = new RecordTypeNode(pos, name);
    field_list(block, node);
    while (scanner_->peek()->type() == TokenType::semicolon) {
        scanner_->next();
        field_list(block, node);
    }
    if (assertToken(scanner_->peek(), TokenType::kw_end)) {
        scanner_->next();
    }
    return node;
}

// field_list = ident_list ":" type .
void Parser::field_list(BlockNode *block, RecordTypeNode *record) {
    logger_->debug("", "field_list");
    std::vector<std::string> idents;
    ident_list(idents);
    auto token = scanner_->next();
    if (assertToken(token.get(), TokenType::colon)) {
        auto node = type(block);
        for (const std::string& ident : idents) {
            record->addField(std::make_unique<FieldNode>(token->pos(), ident, node));
        }
    }
}

// var_declarations =  "VAR" { ident_list ":" type ";" } .
void Parser::var_declarations(BlockNode *block) {
    logger_->debug("", "var_declarations");
    scanner_->next(); // skip VAR keyword
    while (scanner_->peek()->type() == TokenType::const_ident) {
        std::vector<std::string> idents;
        ident_list(idents);
        auto token = scanner_->next();
        auto pos = token->pos();
        if (assertToken(token.get(), TokenType::colon)) {
            auto node = type(block);
            for (auto &&ident : idents) {
                auto variable = std::make_unique<VariableDeclarationNode>(pos, ident, node);
                block->addVariable(std::move(variable));
            }
            token = scanner_->next();
            if (token->type() != TokenType::semicolon) {
                logger_->error(token->pos(), "; expected, found " + to_string(token->type()) + ".");
            }
        }
    }
}

// procedure_declaration = procedure_heading ";" ( procedure_body ident | "EXTERN" ) ";" .
void Parser::procedure_declaration(BlockNode *block) {
    logger_->debug("", "procedure_declaration");
    auto proc = procedure_heading();
    auto token = scanner_->peek();
    if (token->type() != TokenType::semicolon) {
        logger_->error(token->pos(), "; expected, found " + to_string(token->type()) + ".");
    } else {
        scanner_->next();
    }
    if (scanner_->peek()->type() == TokenType::kw_extern) {
        scanner_->next(); // skip EXTERN keyword
        proc->setExtern(true);
    } else {
        procedure_body(proc.get());
        auto name = ident();
        if (name != proc->getName()) {
            logger_->error(token_->pos(), "procedure name mismatch: expected " +
                           proc->getName() + ", found " + name + ".");
        }
    }
    token = scanner_->peek();
    if (token->type() != TokenType::semicolon) {
        logger_->error(token->pos(), "; expected, found " + to_string(token->type()) + ".");
    } else {
        scanner_->next();
    }
    block->addProcedure(std::move(proc));
}

// procedure_heading = "PROCEDURE" ident [ formal_parameters ] [ ":" type ] .
std::unique_ptr<ProcedureNode> Parser::procedure_heading() {
    logger_->debug("", "procedure_heading");
    auto token = scanner_->next(); // skip PROCEDURE keyword
    auto pos = token->pos();
    auto name = ident();
    auto proc = std::make_unique<ProcedureNode>(pos, name);
    if (scanner_->peek()->type() == TokenType::lparen) {
        formal_parameters(proc.get());
    }
    if (scanner_->peek()->type() == TokenType::colon) {
        scanner_->next(); // skip colon
        proc->setReturnType(type(proc.get()));
    }
    return proc;
}

// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
void Parser::procedure_body(ProcedureNode *proc) {
    logger_->debug("", "procedure_body");
    declarations(proc);
    if (scanner_->peek()->type() == TokenType::kw_begin) {
        scanner_->next(); // skip BEGIN keyword
        statement_sequence(proc->getStatements());
    }
    if (assertToken(scanner_->peek(), TokenType::kw_end)) {
        scanner_->next();
    }
}

// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")".
void Parser::formal_parameters(ProcedureNode *proc) {
    logger_->debug("", "formal_parameters");
    auto token = scanner_->next(); // skip left parenthesis
    if (token->type() == TokenType::lparen) {
        TokenType type = scanner_->peek()->type();
        if (type == TokenType::kw_var || type == TokenType::const_ident || type == TokenType::varargs) {
            fp_section(proc);
            while (scanner_->peek()->type() == TokenType::semicolon) {
                token = scanner_->next(); // skip semicolon
                if (proc->hasVarArgs()) {
                    logger_->error(token->pos(), "varargs must be last formal parameter.");
                }
                fp_section(proc);
            }
        }
        token = scanner_->next();
        if (token->type() != TokenType::rparen) {
            logger_->error(token->pos(), ") expected, found " + to_string(token->type()) + ".");
        }
    }
}

// fp_section = ( [ "VAR" ] ident_list ":" type | "..." ) .
void Parser::fp_section(ProcedureNode *proc) {
    logger_->debug("", "fp_section");
    if (scanner_->peek()->type() == TokenType::varargs) {
        scanner_->next(); // skip varargs
        proc->setVarArgs(true);
    } else {
        bool var = false;
        if (scanner_->peek()->type() == TokenType::kw_var) {
            scanner_->next(); // skip VAR keyword
            var = true;
        }
        std::vector<std::string> idents;
        ident_list(idents);
        auto token = scanner_->next();
        if (token->type() != TokenType::colon) {
            logger_->error(token->pos(), ": expected, found " + to_string(token->type()) + ".");
        }
        auto node = type(proc);
        for (auto ident : idents) {
            proc->addParameter(std::make_unique<ParameterNode>(token->pos(), ident, node, var));
        }
    }
}

// statement_sequence = statement { ";" statement } .
void Parser::statement_sequence(StatementSequenceNode *statements) {
    logger_->debug("", "statement_sequence");
    auto token = scanner_->peek();
    if (token->type() == TokenType::kw_end || token->type() == TokenType::kw_elsif ||
        token->type() == TokenType::kw_else || token->type() == TokenType::kw_until) {
        logger_->error(token->pos(), "empty statement sequence.");
    } else {
        while (true) {
            statements->addStatement(statement());
            token = scanner_->peek();
            while (token->type() == TokenType::semicolon) {
                scanner_->next(); // skip semicolon
                statements->addStatement(statement());
                token = scanner_->peek();
            }
            if (token->type() == TokenType::kw_end || token->type() == TokenType::kw_elsif ||
               token->type() == TokenType::kw_else || token->type() == TokenType::kw_until) {
                break;
            } else {
                resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_until });
            }
        }
    }
}

// statement = ( assignment | procedure_call | if_statement | case_statement
//               while_statement | repeat_statement | for_statement | loop_statement
//               with_statement | "EXIT" | "RETURN" [ expression ] ) .
std::unique_ptr<StatementNode> Parser::statement() {
    logger_->debug("", "statement");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        FilePos pos = token->pos();
        auto name = ident();
        token = scanner_->peek();
        if (token->type() == TokenType::op_becomes ||
            token->type() == TokenType::period ||
            token->type() == TokenType::lbrack) {
            auto lvalue = std::make_unique<ValueReferenceNode>(pos, name);
            token = scanner_->peek();
            while (token->type() == TokenType::period || token->type() == TokenType::lbrack) {
                selector(lvalue.get());
                token = scanner_->peek();
            }
            token = scanner_->peek();
            if (assertToken(token, TokenType::op_becomes)) {
                return assignment(std::move(lvalue));
            }
        } else {
            auto call = std::make_unique<ProcedureCallNode>(pos, name);
            procedure_call(call.get());
            return call;
        }
    } else if (token->type() == TokenType::kw_if) {
        return if_statement();
    } else if (token->type() == TokenType::kw_loop) {
        return loop_statement();
    } else if (token->type() == TokenType::kw_while) {
        return while_statement();
    } else if (token->type() == TokenType::kw_repeat) {
        return repeat_statement();
    } else if (token->type() == TokenType::kw_for) {
        return for_statement();
    } else if (token->type() == TokenType::kw_return) {
        token_ = scanner_->next();
        return std::make_unique<ReturnNode>(token_->pos(), expression());
    } else {
        logger_->error(token->pos(), "unknown statement: too many semi-colons?");
    }
    resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_if, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_while });
    return nullptr;
}

// assignment = ident { selector } ":=" expression .
std::unique_ptr<StatementNode> Parser::assignment(std::unique_ptr<ValueReferenceNode> lvalue) {
    logger_->debug("", "assignment");
    scanner_->next(); // skip assign operator
    return std::make_unique<AssignmentNode>(lvalue->pos(), std::move(lvalue), expression());
}

// procedure_call = ident [ actual_parameters ] .
void Parser::procedure_call(ProcedureNodeReference *call) {
    logger_->debug("", "procedure_call");
    if (scanner_->peek()->type() == TokenType::lparen) {
        actual_parameters(call);
    }
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
std::unique_ptr<StatementNode> Parser::if_statement() {
    logger_->debug("", "if_statement");
    token_ = scanner_->next(); // skip IF keyword
    auto condition = expression();
    auto statement = std::make_unique<IfThenElseNode>(token_->pos(), std::move(condition));
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_then)) {
        statement_sequence(statement->addThenStatements(token_->pos()));
        token_ = scanner_->next();
        while (token_->type() == TokenType::kw_elsif) {
            condition = expression();
            token_ = scanner_->next();
            if (assertToken(token_.get(), TokenType::kw_then)) {
                statement_sequence(statement->addElseIf(token_->pos(), std::move(condition)));
            }
            token_ = scanner_->next();
        }
        if (token_->type() == TokenType::kw_else) {
            statement_sequence(statement->addElseStatements(token_->pos()));
            token_ = scanner_->next();
        }
        if (token_->type() != TokenType::kw_end) {
            logger_->error(token_->pos(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    return statement;
}

// loop_statement = "LOOP" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::loop_statement() {
    logger_->debug("", "loop_statement");
    token_ = scanner_->next(); // skip LOOP keyword
    auto statement = std::make_unique<LoopNode>(token_->pos());
    statement_sequence(statement->getStatements());
    token_ = scanner_->next();
    if (token_->type() != TokenType::kw_end) {
        logger_->error(token_->pos(), "END expected, found " + to_string(token_->type()) + ".");
    }
    return statement;
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::while_statement() {
    logger_->debug("", "while_statement");
    token_ = scanner_->next(); // skip WHILE keyword
    auto statement = std::make_unique<WhileLoopNode>(token_->pos(), expression());
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(statement->getStatements());
        token_ = scanner_->next();
        if (token_->type() != TokenType::kw_end) {
            logger_->error(token_->pos(), "END expected, found " + to_string(token_->type()) + ".");
        }
    }
    return statement;
}

// repeat_statement = "REPEAT" statement_sequence "UNTIL" expression .
std::unique_ptr<StatementNode> Parser::repeat_statement() {
    logger_->debug("", "repeat_statement");
    token_ = scanner_->next(); // skip REPEAT keyword
    auto statement = std::make_unique<RepeatLoopNode>(token_->pos());
    statement_sequence(statement->getStatements());
    token_ = scanner_->next();
    if (token_->type() == TokenType::kw_until) {
        statement->setCondition(expression());
    } else {
        logger_->error(token_->pos(), "UNTIL expected.");
    }
    return statement;
}

// for_statement = "FOR" ident ":=" expression "TO" expression [ "BY" const_expression ] "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::for_statement() {
    logger_->debug("", "for_statement");
    token_ = scanner_->next(); // skip FOR keyword
    FilePos pos = scanner_->peek()->pos();
    auto name = ident();
    auto counter = std::make_unique<ValueReferenceNode>(pos, name);
    token_ = scanner_->next();
    std::unique_ptr<ExpressionNode> low = nullptr;
    if (assertToken(token_.get(), TokenType::op_becomes)) {
        low = expression();
    }
    token_ = scanner_->next();
    std::unique_ptr<ExpressionNode> high = nullptr;
    if (assertToken(token_.get(), TokenType::kw_to)) {
        high = expression();
    }
    std::unique_ptr<ExpressionNode> step = nullptr;
    if (scanner_->peek()->type() == TokenType::kw_by) {
        scanner_->next(); // skip BY keyword
        step = expression();
    }
    auto statement = std::make_unique<ForLoopNode>(pos, std::move(counter), std::move(low), std::move(high), std::move(step));
    token_ = scanner_->next();
    if (assertToken(token_.get(), TokenType::kw_do)) {
        statement_sequence(statement->getStatements());
    }
    token_ = scanner_->next();
    if (token_->type() != TokenType::kw_end) {
        logger_->error(token_->pos(), "END expected, found " + to_string(token_->type()) + ".");
    }
    return statement;
}

// actual_parameters = "(" [ expression { "," expression } ] ")" .
void Parser::actual_parameters(ProcedureNodeReference *call) {
    logger_->debug("", "actual_parameters");
    token_ = scanner_->next(); // skip left parenthesis
    if (scanner_->peek()->type() == TokenType::rparen) {
        token_ = scanner_->next();
        return;
    }
    call->addParameter(expression());
    while (scanner_->peek()->type() == TokenType::comma) {
        token_ = scanner_->next(); // skip comma
        call->addParameter(expression());
    }
    token_ = scanner_->next();
    if (token_->type() != TokenType::rparen) {
        logger_->error(token_->pos(), ") expected, found " + to_string(token_->type()) + ".");
    }
}

// selector = "." ident | "[" expression "]" .
void Parser::selector(ValueReferenceNode *ref) {
    logger_->debug("", "selector");
    token_ = scanner_->next();
    if (token_->type() == TokenType::period) {
        auto name = ident();
        ref->addSelector(NodeType::record_type, std::make_unique<ValueReferenceNode>(token_->pos(), name));
    } else if (token_->type() == TokenType::lbrack) {
        auto expr = expression();
        ref->addSelector(NodeType::array_type, std::move(expr));
        token_ = scanner_->next();
        if (token_->type() != TokenType::rbrack) {
            logger_->error(token_->pos(), "] expected, found " + to_string(token_->type()) + ".");
        }
    } else {
        logger_->error(token_->pos(), "selector expected.");
    }
}

// expression = simple_expression [ ( "=" | "#" | "<" | "<=" | ">" | ">=" ) simple_expression ] .
std::unique_ptr<ExpressionNode> Parser::expression() {
    logger_->debug("", "expression");
    auto lhs = simple_expression();
    TokenType token = scanner_->peek()->type();
    if (token == TokenType::op_eq
        || token == TokenType::op_neq
        || token == TokenType::op_lt
        || token == TokenType::op_leq
        || token == TokenType::op_gt
        || token == TokenType::op_geq) {
        token_ = scanner_->next();
        OperatorType op = token_to_operator(token_->type());
        auto rhs = simple_expression();
        return std::make_unique<BinaryExpressionNode>(token_->pos(), op, std::move(lhs), std::move(rhs));
    }
    return lhs;
}

// simple_expression = [ "+" | "-" ] term { ( "+" | "-" | OR ) term } .
std::unique_ptr<ExpressionNode> Parser::simple_expression() {
    logger_->debug("", "simple_expression");
    std::unique_ptr<ExpressionNode> expr;
    TokenType token = scanner_->peek()->type();
    if (token == TokenType::op_plus) {
        token_ = scanner_->next();
        expr = term();
    } else if (token == TokenType::op_minus) {
        token_ = scanner_->next();
        expr = std::make_unique<UnaryExpressionNode>(token_->pos(), OperatorType::NEG, term());
    } else {
        expr = term();
    }
    token = scanner_->peek()->type();
    while (token == TokenType::op_plus
           || token == TokenType::op_minus
           || token == TokenType::op_or) {
        token_ = scanner_->next();
        OperatorType op = token_to_operator(token_->type());
        expr = std::make_unique<BinaryExpressionNode>(token_->pos(), op, std::move(expr), term());
        token = scanner_->peek()->type();
    }
    return expr;
}

// term = factor { ( "*" | "DIV" | "MOD" | "&" ) factor } .
std::unique_ptr<ExpressionNode> Parser::term() {
    logger_->debug("", "term");
    auto expr = factor();
    TokenType token = scanner_->peek()->type();
    while (token == TokenType::op_times
           || token == TokenType::op_div
           || token == TokenType::op_mod
           || token == TokenType::op_and) {
        token_ = scanner_->next();
        OperatorType op = token_to_operator(token_->type());
        expr = std::make_unique<BinaryExpressionNode>(token_->pos(), op, std::move(expr), factor());
        token = scanner_->peek()->type();
    }
    return expr;
}

// factor = ident [ actural_parameters ] { selector } | integer | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
std::unique_ptr<ExpressionNode> Parser::factor() {
    logger_->debug("", "factor");
    auto token = scanner_->peek();
    if (token->type() == TokenType::const_ident) {
        FilePos pos = token->pos();
        std::string name = ident();
        token = scanner_->peek();
        std::unique_ptr<ValueReferenceNode> ref;
        if (token->type() == TokenType::lparen) {
            auto call = std::make_unique<FunctionCallNode>(pos, name);
            actual_parameters(call.get());
            ref = std::move(call);
        } else {
            ref = std::make_unique<ValueReferenceNode>(pos, name);
        }
        token = scanner_->peek();
        while (token->type() == TokenType::period || token->type() == TokenType::lbrack) {
            selector(ref.get());
            token = scanner_->peek();
        }
        return ref;
    } else if (token->type() == TokenType::integer_literal) {
        auto tmp = scanner_->next();
        auto number = dynamic_cast<const IntegerLiteralToken*>(tmp.get());
        return std::make_unique<IntegerLiteralNode>(number->pos(), number->value());
    } else if (token->type() == TokenType::string_literal) {
        auto tmp = scanner_->next();
        auto string = dynamic_cast<const StringLiteralToken*>(tmp.get());
        return std::make_unique<StringLiteralNode>(string->pos(), string->value());
    } else if (token->type() == TokenType::boolean_literal) {
        auto tmp = scanner_->next();
        auto boolean = dynamic_cast<const BooleanLiteralToken*>(tmp.get());
        return std::make_unique<BooleanLiteralNode>(boolean->pos(), boolean->value());
    } else if (token->type() == TokenType::lparen) {
        scanner_->next();
        auto expr = expression();
        token = scanner_->peek();
        if (assertToken(token, TokenType::rparen)) {
            scanner_->next();
        }
        return expr;
    } else if (token->type() == TokenType::op_not) {
        scanner_->next();
        return std::make_unique<UnaryExpressionNode>(token->pos(), OperatorType::NOT, factor());
    } else {
        logger_->error(token->pos(), "unexpected token: " + to_string(token->type()) + ".");
        resync({ TokenType::semicolon });
        return nullptr;
    }
}

bool Parser::assertToken(const Token *token, TokenType expected) {
    if (token->type() == expected) {
        return true;
    }
    logger_->error(token->pos(), to_string(expected) + " expected, found " + to_string(token->type()) + ".");
    return false;
}

void Parser::resync(std::set<TokenType> types) {
    auto type = scanner_->peek()->type();
    while (types.find(type) == types.end()) {
        scanner_->next();
        type = scanner_->peek()->type();
    }
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
