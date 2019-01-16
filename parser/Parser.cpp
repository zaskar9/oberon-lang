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
#include "ast/DeclarationNode.h"
#include "ast/ReferenceNode.h"
#include "ast/IfThenElseNode.h"
#include "ast/LoopNode.h"
#include "ast/AssignmentNode.h"
#include "ast/ProcedureCallNode.h"

static OperatorType token_to_operator(TokenType token);

Parser::Parser(Scanner *scanner, Logger *logger) : scanner_(scanner), logger_(logger) {
    symbols_ = std::make_unique<SymbolTable>();
}

Parser::~Parser() = default;

std::unique_ptr<ModuleNode> Parser::parse() {
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

// ident_list = ident { "," identifier } .
void Parser::ident_list(std::vector<std::string> &idents) {
    logger_->debug("", "ident_list");
    idents.push_back(ident());
    while (scanner_->peekToken()->getType() == TokenType::comma) {
        scanner_->nextToken(); // skip comma
        idents.push_back(ident());
    }
}

// module = "MODULE" identifier ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
std::unique_ptr<ModuleNode> Parser::module() {
    logger_->debug("", "module");
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_module) {
        auto module = std::make_unique<ModuleNode>(token->getPosition(), ident(), symbols_->getLevel());
        token = scanner_->nextToken();
        if (token->getType() == TokenType::semicolon) {
            symbols_->enterScope();
            declarations(module.get());
            token = scanner_->nextToken();
            if (token->getType() == TokenType::kw_begin) {
                statement_sequence(module->getStatements());
                token = scanner_->nextToken();
            }
            if (token->getType() == TokenType::kw_end) {
                auto name = ident();
                if (name == module->getName()) {
                    token = scanner_->nextToken();
                    if (token->getType() == TokenType::period) {
                        symbols_->leaveScope();
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
    symbols_->leaveScope();
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
                auto constant = std::make_unique<ConstantDeclarationNode>(token->getPosition(), name, std::move(value),
                        symbols_->getLevel());
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
        auto pos = scanner_->peekToken()->getPosition();
        std::string name = ident();
        if (symbols_->isDuplicate(name)) {
            logger_->error(pos, "duplicate definition: " + name);
        }
        auto token = scanner_->nextToken();
        if (token->getType() == TokenType::op_eq) {
            auto node = std::make_unique<TypeDeclarationNode>(pos, name, type(block), symbols_->getLevel());
            symbols_->insert(name, node.get());
            block->addTypeDeclaration(std::move(node));
            token = scanner_->nextToken();
            if (token->getType() != TokenType::semicolon) {
                logger_->error(token->getPosition(), "; expected.");
            }
        } else {
            logger_->error(token->getPosition(), "= expected.");
        }
    }
}

// type = identifier | array_type | record_type .
TypeNode* Parser::type(BlockNode *block) {
    logger_->debug("", "type");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::const_ident) {
        auto pos = token->getPosition();
        std::string name = ident();
        auto node = symbols_->lookup(name);
        if (node == nullptr) {
            logger_->error(pos, "undefined type: " + name + ".");
        } else if (node->getNodeType() == NodeType::array_type ||
                   node->getNodeType() == NodeType::basic_type ||
                   node->getNodeType() == NodeType::record_type) {
            return dynamic_cast<TypeNode*>(node);
        } else if (node->getNodeType() == NodeType::type_declaration) {
            auto type = dynamic_cast<TypeDeclarationNode*>(node);
            return type->getType();
        } else {
            logger_->error(pos, name + " is not a type.");
        }
    } else if (token->getType() == TokenType::kw_array) {
        std::unique_ptr<ArrayTypeNode> node(array_type(block));
        auto res = node.get();
        block->addType(std::move(node));
        return res;
    } else if (token->getType() == TokenType::kw_record) {
        std::unique_ptr<RecordTypeNode> node(record_type(block));
        auto res = node.get();
        block->addType(std::move(node));
        return res;
    } else {
        logger_->error(token->getPosition(), "unexpected token.");
        resync({ TokenType::semicolon, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin });
    }
    return nullptr;
}

// array_type = "ARRAY" expression "OF" type .
ArrayTypeNode* Parser::array_type(BlockNode *block) {
    logger_->debug("", "array_type");
    FilePos pos = scanner_->nextToken()->getPosition(); // skip ARRAY keyword and get its position
    auto expr = expression();
    if (expr != nullptr) {
        if (expr->isConstant() && expr->getType()==BasicTypeNode::INTEGER) {
            int dim = foldNumber(expr.get());
            auto token = scanner_->peekToken();
            if (token->getType()==TokenType::kw_of) {
                scanner_->nextToken(); // skip OF keyword
                return new ArrayTypeNode(pos, dim, type(block));
            }
            else {
                logger_->error(token->getPosition(), "OF expected.");
                resync({ TokenType::semicolon, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin });
            }
        }
        else {
            logger_->error(expr->getFilePos(), "integer expression expected.");
        }
    } else {
        logger_->error(scanner_->peekToken()->getPosition(), "integer expression expected.");
    }
    return nullptr;
}

// record_type = "RECORD" field_list { ";" field_list } "END" .
RecordTypeNode* Parser::record_type(BlockNode *block) {
    logger_->debug("", "record_type");
    FilePos pos = scanner_->nextToken()->getPosition(); // skip RECORD keyword and get its position
    auto node = new RecordTypeNode(pos);
    field_list(block, node);
    while (scanner_->peekToken()->getType() == TokenType::semicolon) {
        field_list(block, node);
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
void Parser::field_list(BlockNode *block, RecordTypeNode *record) {
    logger_->debug("", "field_list");
    std::vector<std::string> idents;
    ident_list(idents);
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::colon) {
        auto node = type(block);
        for (const std::string& ident : idents) {
            if (record->getField(ident) == nullptr) {
                record->addField(std::make_unique<FieldNode>(token->getPosition(), ident, node, record->getOffset()));
                record->incOffset(node->getSize());
            } else {
                logger_->error(token->getPosition(), "duplicate record field: " + ident + ".");
            }
        }
    } else {
        logger_->error(token->getPosition(), ": expected.");
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
            auto node = type(block);
            if (node != nullptr) {
                for (auto &&ident : idents) {
                    if (symbols_->isDuplicate(ident)) {
                        logger_->error(token->getPosition(), "duplicate definition: " + ident);
                        continue;
                    }
                    auto variable = std::make_unique<VariableDeclarationNode>(pos, ident, node,
                                                                              symbols_->getLevel(), block->getOffset());
                    symbols_->insert(ident, variable.get());
                    block->addVariable(std::move(variable));
                    block->incOffset(node->getSize());
                }
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
    auto proc = procedure_heading();
    auto token = scanner_->nextToken();
    if (token->getType() != TokenType::semicolon) {
        logger_->error(token->getPosition(), "; expected.");
    }
    procedure_body(proc.get());
    auto name = ident();
    token = scanner_->nextToken();
    if (token->getType() != TokenType::semicolon) {
        logger_->error(token->getPosition(),"; expected.");
    }
    if (name != proc->getName()) {
        logger_->error(token->getPosition(), proc->getName() + " expected.");
    }
    symbols_->leaveScope();
    block->addProcedure(std::move(proc));
}

// procedure_heading = "PROCEDURE" identifier [ formal_parameters ] .
std::unique_ptr<ProcedureNode> Parser::procedure_heading() {
    logger_->debug("", "procedure_heading");
    auto token = scanner_->nextToken(); // skip PROCEDURE keyword
    auto proc = std::make_unique<ProcedureNode>(token->getPosition(), ident(), symbols_->getLevel());
    symbols_->insert(proc->getName(), proc.get());
    symbols_->enterScope();
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
        statement_sequence(proc->getStatements());
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
    auto node = type(proc);
    for (auto ident : idents) {
        auto param = std::make_unique<ParameterNode>(token->getPosition(), ident, node, var, symbols_->getLevel());
        symbols_->insert(ident, param.get());
        proc->addParameter(std::move(param));
    }
}

// statement_sequence = statement { ";" statement } .
void Parser::statement_sequence(StatementSequenceNode *statements) {
    logger_->debug("", "statement_sequence");
    statements->addStatement(statement());
    while (scanner_->peekToken()->getType() == TokenType::semicolon) {
        scanner_->nextToken(); // skip semicolon
        statements->addStatement(statement());
    }
}

// statement = [ assignment | procedure_call | if_statement | case_statement
//               while_statement | repeat_statement | for_statement | loop_statement
//               with_statement | "EXIT" | "RETURN" [ expression ] ] .
std::unique_ptr<StatementNode> Parser::statement() {
    logger_->debug("", "statement");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::const_ident) {
        FilePos pos = token->getPosition();
        auto name = ident();
        auto symbol = symbols_->lookup(name);
        if (symbol == nullptr) {
            logger_->error(pos, "undefined identifier: " + name + ".");
        } else if (symbol->getNodeType() == NodeType::variable || symbol->getNodeType() == NodeType::parameter) {
            auto var = dynamic_cast<DeclarationNode*>(symbol);
            token = scanner_->peekToken();
            std::unique_ptr<ReferenceNode> lvalue;
            if (token->getType() == TokenType::period || token->getType() == TokenType::lbrack) {
                lvalue = std::make_unique<ReferenceNode>(pos, var, selector(var));
            } else {
                lvalue = std::make_unique<ReferenceNode>(pos, var);
            }
            token = scanner_->peekToken();
            if (token->getType() == TokenType::op_becomes) {
                return assignment(std::move(lvalue));
            }
        } else if (symbol->getNodeType() == NodeType::procedure) {
            auto call = std::make_unique<ProcedureCallNode>(pos, dynamic_cast<ProcedureNode*>(symbol));
            procedure_call(call.get());
            return call;
        } else if (symbol->getNodeType() == NodeType::constant) {
            logger_->error(pos, "constant cannot be assigned: " + name + ".");
        } else {
            logger_->error(pos, "variable or procedure name expected, found: " + name + ".");
        }
    } else if (token->getType() == TokenType::kw_if) {
        return if_statement();
    } else if (token->getType() == TokenType::kw_loop) {
        return loop_statement();
    } else if (token->getType() == TokenType::kw_while) {
        return while_statement();
    } else if (token->getType() == TokenType::kw_repeat) {
        return repeat_statement();
    } else if (token->getType() == TokenType::kw_for) {
        return for_statement();
    } else {
        logger_->error(token->getPosition(), "unknown statement: too many semi-colons?");
    }
    resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_if, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_while });
    return nullptr;
}

// assignment = identifier selector ":=" expression .
std::unique_ptr<StatementNode> Parser::assignment(std::unique_ptr<ReferenceNode> lvalue) {
    logger_->debug("", "assignment");
    auto token = scanner_->nextToken(); // skip becomes
    auto expr = expression();
    if (lvalue->getType() == expr->getType()) {
        return std::make_unique<AssignmentNode>(lvalue->getFilePos(), std::move(lvalue), std::move(expr));
    }
    logger_->error(token->getPosition(), "illegal assignment: type mismatch.");
    return nullptr;
}

// procedure_call = identifier [ actual_parameters ] .
void Parser::procedure_call(ProcedureCallNode *call) {
    logger_->debug("", "procedure_call");
    if (scanner_->peekToken()->getType() == TokenType::lparen) {
        actual_parameters(call);
    }
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
std::unique_ptr<StatementNode> Parser::if_statement() {
    logger_->debug("", "if_statement");
    auto token = scanner_->nextToken(); // skip IF keyword
    auto condition = expression();
    if (condition->getType() != BasicTypeNode::BOOLEAN) {
        logger_->error(condition->getFilePos(), "Boolean expression expected.");
    }
    auto statement = std::make_unique<IfThenElseNode>(token->getPosition(), std::move(condition));
    token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_then) {
        statement_sequence(statement->addThenStatements(token->getPosition()));
        token = scanner_->nextToken();
        while (token->getType() == TokenType::kw_elsif) {
            // scanner_->nextToken(); // skip ELSIF keyword
            condition = expression();
            if (condition->getType() != BasicTypeNode::BOOLEAN) {
                logger_->error(condition->getFilePos(), "Boolean expression expected.");
            }
            token = scanner_->nextToken();
            if (token->getType() == TokenType::kw_then) {
                statement_sequence(statement->addElseIf(token->getPosition(), std::move(condition)));
            } else {
                logger_->error(token->getPosition(), "THEN expected, instead of " + to_string(token->getType()) + ".");
            }
            token = scanner_->nextToken();
        }
        if (token->getType() == TokenType::kw_else) {
            statement_sequence(statement->addElseStatements(token->getPosition()));
            token = scanner_->nextToken();
        }
        if (token->getType() != TokenType::kw_end) {
            logger_->error(token->getPosition(), "END expected.");
        }
    } else {
        logger_->error(token->getPosition(), "THEN expected, instead of " + to_string(token->getType()) + ".");
    }
    return statement;
}

// loop_statement = "LOOP" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::loop_statement() {
    logger_->debug("", "loop_statement");
    auto token = scanner_->nextToken(); // skip LOOP keyword
    auto statement = std::make_unique<LoopNode>(token->getPosition());
    statement_sequence(statement->getStatements());
    token = scanner_->nextToken();
    if (token->getType() != TokenType::kw_end) {
        logger_->error(token->getPosition(), "END expected.");
    }
    return statement;
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::while_statement() {
    logger_->debug("", "while_statement");
    auto token = scanner_->nextToken(); // skip WHILE keyword
    auto condition = expression();
    if (condition->getType() != BasicTypeNode::BOOLEAN) {
        logger_->error(condition->getFilePos(), "Boolean expression expected.");
    }
    auto statement = std::make_unique<WhileLoopNode>(token->getPosition(), std::move(condition));
    token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_do) {
        statement_sequence(statement->getStatements());
        token = scanner_->nextToken();
        if (token->getType() != TokenType::kw_end) {
            logger_->error(token->getPosition(), "END expected.");
        }
    } else {
        logger_->error(token->getPosition(), "DO expected.");
    }
    return statement;
}

// repeat_statement = "REPEAT" statement_sequence "UNTIL" expression .
std::unique_ptr<StatementNode> Parser::repeat_statement() {
    logger_->debug("", "repeat_statement");
    auto token = scanner_->nextToken(); // skip REPEAT keyword
    auto statement = std::make_unique<RepeatLoopNode>(token->getPosition());
    statement_sequence(statement->getStatements());
    token = scanner_->nextToken();
    if (token->getType() == TokenType::kw_until) {
        auto condition = expression();
        if (condition->getType() != BasicTypeNode::BOOLEAN) {
            logger_->error(condition->getFilePos(), "Boolean expression expected.");
        }
        statement->setCondition(std::move(condition));
    } else {
        logger_->error(token->getPosition(), "UNTIL expected.");
    }
    return statement;
}

// for_statement = "FOR" ident ":=" expression "TO" expression [ "BY" const_expression ] "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::for_statement() {
    logger_->debug("", "for_statement");
    auto token = scanner_->nextToken(); // skip FOR keyword
    FilePos pos = scanner_->peekToken()->getPosition();
    auto name = ident();
    auto symbol = symbols_->lookup(name);
    if (symbol == nullptr) {
        logger_->error(pos, "undefined identifier: " + name + ".");
    } else if (symbol->getNodeType() == NodeType::variable) {
        auto var = dynamic_cast<VariableDeclarationNode*>(symbol);
        if (var->getType()!=BasicTypeNode::INTEGER) {
            logger_->error(pos, "integer variable expected.");
        }
        auto counter = std::make_unique<ReferenceNode>(pos, var);
        std::unique_ptr<ExpressionNode> low = nullptr;
        std::unique_ptr<ExpressionNode> high = nullptr;
        int step = 1;
        token = scanner_->nextToken();
        if (token->getType()==TokenType::op_becomes) {
            low = expression();
            if (low->getType() != BasicTypeNode::INTEGER) {
                logger_->error(low->getFilePos(), "integer expression expected.");
            }
        }
        else {
            logger_->error(token->getPosition(), ":= expected");
        }
        token = scanner_->nextToken();
        if (token->getType()==TokenType::kw_to) {
            high = expression();
            if (high->getType() != BasicTypeNode::INTEGER) {
                logger_->error(high->getFilePos(), "integer expression expected.");
            }
        }
        else {
            logger_->error(token->getPosition(), "TO expected.");
        }
        if (scanner_->peekToken()->getType()==TokenType::kw_by) {
            scanner_->nextToken(); // skip BY keyword
            auto expr = expression();
            if (expr->getType()==BasicTypeNode::INTEGER && expr->isConstant()) {
                step = foldNumber(expr.get());
            }
            else {
                logger_->error(expr->getFilePos(), "constant integer expression expected.");
            }
        }
        auto statement = std::make_unique<ForLoopNode>(pos, std::move(counter), std::move(low), std::move(high), step);
        token = scanner_->nextToken();
        if (token->getType()==TokenType::kw_do) {
            statement_sequence(statement->getStatements());
        }
        else {
            logger_->error(token->getPosition(), "DO expected.");
        }
        token = scanner_->nextToken();
        if (token->getType()!=TokenType::kw_end) {
            logger_->error(token->getPosition(), "END expected.");
        }
        return statement;
    } else {
        logger_->error(pos, name + " cannot be used as a loop counter.");
    }
    return nullptr;
}

// actual_parameters = "(" [ expression { "," expression } ] ")" .
void Parser::actual_parameters(ProcedureCallNode *call) {
    logger_->debug("", "actual_parameters");
    scanner_->nextToken(); // skip left parenthesis
    if (scanner_->peekToken()->getType() == TokenType::rparen) {
        scanner_->nextToken();
        return;
    }
    size_t num = 0;
    auto expr = expression();
    if (checkActualParameter(call->getProcedure(), num, expr.get())) {
        call->addParameter(std::move(expr));
    }
    while (scanner_->peekToken()->getType() == TokenType::comma) {
        scanner_->nextToken(); // skip comma
        num++;
        expr = expression();
        if (checkActualParameter(call->getProcedure(), num, expr.get())) {
            call->addParameter(std::move(expr));
        }
    }
    auto token = scanner_->nextToken();
    if (token->getType() != TokenType::rparen) {
        logger_->error(token->getPosition(), ") expected.");
    }
    if (num + 1 < call->getProcedure()->getParameterCount()) {
        logger_->error(token->getPosition(), "fewer actual than formal parameters.");
    }
}

// selector = {"." identifier | "[" expression "]"}.
std::unique_ptr<ExpressionNode> Parser::selector(const DeclarationNode *variable) {
    logger_->debug("", "selector");
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::period) {
        auto name = ident();
        if (variable->getType()->getNodeType() == NodeType::record_type) {
            auto record = dynamic_cast<const RecordTypeNode*>(variable->getType());
            auto field = record->getField(name);
            if (field != nullptr) {
                return std::make_unique<ReferenceNode>(token->getPosition(), field);
            } else {
                logger_->error(token->getPosition(), "unknown record field: " + name + ".");
            }
        } else {
            logger_->error(token->getPosition(), "variable or parameter of RECORD type expected.");
        }
    } else if (token->getType() == TokenType::lbrack) {
        auto expr = expression();
        if (expr->getType() != BasicTypeNode::INTEGER) {
            logger_->error(expr->getFilePos(), "integer expression expected.");
        }
        token = scanner_->nextToken();
        if (token->getType() != TokenType::rbrack) {
            logger_->error(token->getPosition(), "] expected.");
        }
        return expr;
    }
    return nullptr;
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
        auto expr = std::make_unique<BinaryExpressionNode>(token->getPosition(), op, std::move(lhs), std::move(rhs));
        if (expr->getLeftExpression()->isConstant() && expr->getRightExpression()->isConstant()) {
            return fold(expr.get());
        }
        return expr;
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
        auto temp = std::make_unique<BinaryExpressionNode>(token->getPosition(), op, std::move(expr), term());
        auto lhs = temp->getLeftExpression();
        auto rhs = temp->getRightExpression();
        if (lhs != nullptr && lhs->isConstant() &&
            rhs != nullptr && rhs->isConstant()) {
            expr = fold(temp.get());
        } else {
            expr = std::move(temp);
        }
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
        auto temp = std::make_unique<BinaryExpressionNode>(token->getPosition(), op, std::move(expr), factor());
        auto lhs = temp->getLeftExpression();
        auto rhs = temp->getRightExpression();
        if (lhs != nullptr && lhs->isConstant() &&
            rhs != nullptr && rhs->isConstant()) {
            expr = fold(temp.get());
        } else {
            expr = std::move(temp);
        }
        type = scanner_->peekToken()->getType();
    }
    return expr;
}

// factor = identifier { selector } | integer | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
std::unique_ptr<ExpressionNode> Parser::factor() {
    logger_->debug("", "factor");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::const_ident) {
        FilePos pos = token->getPosition();
        std::string name = ident();
        auto node = symbols_->lookup(name);
        if (node != nullptr) {
            if (node->getNodeType() == NodeType::constant) {
                auto constant = dynamic_cast<ConstantDeclarationNode*>(node);
                auto value = constant->getValue();
                switch (value->getNodeType()) {
                    case NodeType::boolean:
                        BooleanLiteralNode* boolean;
                        boolean = dynamic_cast<BooleanLiteralNode*>(value);
                        return std::make_unique<BooleanLiteralNode>(pos, boolean->getValue());
                    case NodeType::integer:
                        IntegerLiteralNode* number;
                        number = dynamic_cast<IntegerLiteralNode*>(value);
                        return std::make_unique<IntegerLiteralNode>(pos, number->getValue());
                    case NodeType::string:
                        StringLiteralNode* string;
                        string = dynamic_cast<StringLiteralNode*>(value);
                        return std::make_unique<StringLiteralNode>(pos, string->getValue());
                    default:
                        logger_->error(pos, "unknown type of constant value");
                        return nullptr;
                }
            } else if (node->getNodeType() == NodeType::parameter || node->getNodeType() == NodeType::variable) {
                auto var = dynamic_cast<DeclarationNode*>(node);
                token = scanner_->peekToken();
                if (token->getType() == TokenType::period || token->getType() == TokenType::lbrack) {
                    return std::make_unique<ReferenceNode>(pos, var, selector(var));
                }
                return std::make_unique<ReferenceNode>(pos, var);
            } else {
                logger_->error(pos, "constant, parameter or variable expected.");
                return nullptr;
            }
        } else {
            logger_->error(pos, "undefined identifier: " + name + ".");
            return nullptr;
        }
    } else if (token->getType() == TokenType::const_number) {
        auto tmp = scanner_->nextToken();
        auto number = dynamic_cast<const NumberToken*>(tmp.get());
        return std::make_unique<IntegerLiteralNode>(number->getPosition(), number->getValue());
    } else if (token->getType() == TokenType::const_string) {
        auto tmp = scanner_->nextToken();
        auto string = dynamic_cast<const StringToken*>(tmp.get());
        return std::make_unique<StringLiteralNode>(string->getPosition(), string->getValue());
    } else if (token->getType() == TokenType::const_true) {
        scanner_->nextToken();
        return std::make_unique<BooleanLiteralNode>(token->getPosition(), true);
    } else if (token->getType() == TokenType::const_false) {
        scanner_->nextToken();
        return std::make_unique<BooleanLiteralNode>(token->getPosition(), false);
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

std::unique_ptr<LiteralNode> Parser::fold(const ExpressionNode *expr) const {
    auto type = expr->getType();
    if (type == BasicTypeNode::INTEGER) {
        return std::make_unique<IntegerLiteralNode>(expr->getFilePos(), foldNumber(expr));
    } else if (type == BasicTypeNode::BOOLEAN) {
        return std::make_unique<BooleanLiteralNode>(expr->getFilePos(), foldBoolean(expr));
    } else if (type == BasicTypeNode::STRING) {
        return std::make_unique<StringLiteralNode>(expr->getFilePos(), foldString(expr));
    } else {
        logger_->error(expr->getFilePos(), "incompatible types.");
        return nullptr;
    }
}

int Parser::foldNumber(const ExpressionNode *expr) const {
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
    } else if (expr->getNodeType() == NodeType::integer) {
        auto numConst = dynamic_cast<const IntegerLiteralNode *>(expr);
        return numConst->getValue();
    } else if (expr->getNodeType() == NodeType::name_reference) {
        auto ref = dynamic_cast<const ReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode*>(ref->dereference());
            return foldNumber(constant->getValue());
        }
    } else {
        logger_->error(expr->getFilePos(), "incompatible expression.");
    }
    return 0;
}

bool Parser::foldBoolean(const ExpressionNode *expr) const {
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
        auto boolConst = dynamic_cast<const BooleanLiteralNode*>(expr);
        return boolConst->getValue();
    } else if (expr->getNodeType() == NodeType::name_reference) {
        auto ref = dynamic_cast<const ReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode*>(ref->dereference());
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
        auto stringConst = dynamic_cast<const StringLiteralNode *>(expr);
        return stringConst->getValue();
    } else if (expr->getNodeType() == NodeType::constant) {
        auto ref = dynamic_cast<const ReferenceNode*>(expr);
        if (ref->isConstant()) {
            auto constant = dynamic_cast<const ConstantDeclarationNode*>(ref->dereference());
            foldString(constant->getValue());

        }
    } else {
        logger_->error(expr->getFilePos(), "incompatible expression.");
    }
    return "";
}

bool Parser::checkActualParameter(const ProcedureNode* proc, size_t num, const ExpressionNode* expr) {
    if (num >= proc->getParameterCount()) {
        logger_->error(expr->getFilePos(), "more actual than formal parameters.");
        return false;
    }
    auto parameter = proc->getParameter(num);
    if (parameter->getType() == expr->getType()) {
        if (parameter->isVar()) {
            if (expr->getNodeType() == NodeType::name_reference) {
                auto reference = dynamic_cast<const ReferenceNode*>(expr);
                auto value = reference->dereference();
                if (value->getNodeType() == NodeType::constant) {
                    logger_->error(expr->getFilePos(), "illegal actual parameter: cannot pass constant by reference.");
                    return false;
                }
                return true;
            }
            logger_->error(expr->getFilePos(), "illegal actual parameter: cannot pass expression by reference.");
            return false;
        }
        return true;
    }
    logger_->error(expr->getFilePos(), "illegal actual parameter: type mismatch.");
    return false;
}

void Parser::resync(std::set<TokenType> types) {
    auto type = scanner_->peekToken()->getType();
    while (types.find(type) == types.end()) {
        scanner_->nextToken();
        type = scanner_->peekToken()->getType();
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
