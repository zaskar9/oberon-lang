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
#include "ast/CallNode.h"

static OperatorType token_to_operator(TokenType token);

Parser::Parser(Scanner *scanner, Logger *logger) : scanner_(scanner), logger_(logger), token_() {
    symbols_ = std::make_unique<SymbolTable>();
}

Parser::~Parser() = default;

std::unique_ptr<ModuleNode> Parser::parse() {
    return module();
}

std::string Parser::ident() {
    token_ = scanner_->nextToken();
    if (token_->getType() == TokenType::const_ident) {
        auto ident = dynamic_cast<const IdentToken*>(token_.get());
        logger_->debug("", to_string(*ident));
        return ident->getValue();
    } else {
        logger_->error(token_->getPosition(), "identifier expected.");
    }
    return "";
}

// ident_list = ident { "," identifier } .
void Parser::ident_list(std::vector<std::string> &idents) {
    logger_->debug("", "ident_list");
    idents.push_back(ident());
    while (scanner_->peekToken()->getType() == TokenType::comma) {
        token_ = scanner_->nextToken(); // skip comma
        idents.push_back(ident());
    }
}

// module = "MODULE" identifier ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
std::unique_ptr<ModuleNode> Parser::module() {
    logger_->debug("", "module");
    token_ = scanner_->nextToken();
    if (token_->getType() == TokenType::kw_module) {
        auto module = std::make_unique<ModuleNode>(token_->getPosition(), ident(), symbols_->getLevel());
        token_ = scanner_->nextToken();
        if (token_->getType() == TokenType::semicolon) {
            symbols_->enterScope();
            declarations(module.get());
            token_ = scanner_->nextToken();
            if (token_->getType() == TokenType::kw_begin) {
                statement_sequence(module.get(), module->getStatements());
                token_ = scanner_->nextToken();
            }
            if (token_->getType() == TokenType::kw_end) {
                auto name = ident();
                if (name == module->getName()) {
                    token_ = scanner_->nextToken();
                    if (token_->getType() == TokenType::period) {
                        symbols_->leaveScope();
                        return module;
                    } else {
                        logger_->error(token_->getPosition(), ". expected.");
                    }
                } else {
                    logger_->error(scanner_->peekToken()->getPosition(), "module name mismatch: expected \"" +
                                   module->getName() + "\", found \"" + name + "\".");
                }
            } else {
                logger_->error(token_->getPosition(), "END expected.");
            }
        } else {
            logger_->error(token_->getPosition(), "; expected.");
        }
    } else {
        logger_->error(token_->getPosition(), "MODULE expected.");
    }
    symbols_->leaveScope();
    return nullptr;
}

// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { ProcedureDeclaration } .
void Parser::declarations(BlockNode *parent) {
    logger_->debug("", "declarations");
    if (scanner_->peekToken()->getType() == TokenType::kw_const) {
        const_declarations(parent);
    }
    if (scanner_->peekToken()->getType() == TokenType::kw_type) {
        type_declarations(parent);
    }
    if (scanner_->peekToken()->getType() == TokenType::kw_var) {
        var_declarations(parent);
    }
    while (scanner_->peekToken()->getType() == TokenType::kw_procedure) {
        procedure_declaration(parent);
    }
}

// const_declarations = "CONST" { identifier "=" expression ";" } .
void Parser::const_declarations(BlockNode *parent) {
    logger_->debug("", "const_declarations");
    scanner_->nextToken(); // skip CONST keyword
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
        const std::string name = ident();
        if (symbols_->isDuplicate(name)) {
            logger_->error(scanner_->peekToken()->getPosition(), "duplicate definition: " + name);
        }
        auto token = scanner_->nextToken();
        if (token->getType() == TokenType::op_eq) {
            auto expr = expression(parent);
            if (expr->isConstant()) {
                auto value = fold(expr.get());
                auto constant = std::make_unique<ConstantDeclarationNode>(token->getPosition(), parent, name,
                                                                          std::move(value), symbols_->getLevel());
                symbols_->insert(name, constant.get());
                parent->addConstant(std::move(constant));
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
void Parser::type_declarations(BlockNode *parent) {
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
            auto node = std::make_unique<TypeDeclarationNode>(pos, parent, name, type(parent), symbols_->getLevel());
            symbols_->insert(name, node.get());
            parent->addTypeDeclaration(std::move(node));
            token = scanner_->nextToken();
            if (token->getType() != TokenType::semicolon) {
                logger_->error(token->getPosition(), "; expected.");
            }
        } else {
            logger_->error(token->getPosition(), "= expected.");
        }
    }
}

// type = ( identifier | array_type | record_type ) .
TypeNode* Parser::type(BlockNode *parent) {
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
        std::unique_ptr<ArrayTypeNode> node(array_type(parent));
        auto res = node.get();
        parent->addType(std::move(node));
        return res;
    } else if (token->getType() == TokenType::kw_record) {
        std::unique_ptr<RecordTypeNode> node(record_type(parent));
        auto res = node.get();
        parent->addType(std::move(node));
        return res;
    } else {
        logger_->error(token->getPosition(), "unexpected token.");
        resync({ TokenType::semicolon, TokenType::kw_var, TokenType::kw_procedure, TokenType::kw_begin });
    }
    return nullptr;
}

// array_type = "ARRAY" expression "OF" type .
ArrayTypeNode* Parser::array_type(BlockNode *parent) {
    logger_->debug("", "array_type");
    FilePos pos = scanner_->nextToken()->getPosition(); // skip ARRAY keyword and get its position
    auto expr = expression(parent);
    if (expr != nullptr) {
        if (expr->isConstant() && expr->getType()==BasicTypeNode::INTEGER) {
            int dim = foldNumber(expr.get());
            auto token = scanner_->peekToken();
            if (token->getType()==TokenType::kw_of) {
                scanner_->nextToken(); // skip OF keyword
                return new ArrayTypeNode(pos, dim, type(parent));
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
RecordTypeNode* Parser::record_type(BlockNode *parent) {
    logger_->debug("", "record_type");
    FilePos pos = scanner_->nextToken()->getPosition(); // skip RECORD keyword and get its position
    auto node = new RecordTypeNode(pos);
    field_list(parent, node);
    while (scanner_->peekToken()->getType() == TokenType::semicolon) {
        scanner_->nextToken();
        field_list(parent, node);
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
void Parser::field_list(BlockNode *parent, RecordTypeNode *record) {
    logger_->debug("", "field_list");
    std::vector<std::string> idents;
    ident_list(idents);
    auto token = scanner_->nextToken();
    if (token->getType() == TokenType::colon) {
        auto node = type(parent);
        for (const std::string& ident : idents) {
            if (record->getField(ident) == nullptr) {
                record->addField(std::make_unique<FieldNode>(token->getPosition(), parent, ident, node, record->getOffset()));
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
void Parser::var_declarations(BlockNode *parent) {
    logger_->debug("", "var_declarations");
    scanner_->nextToken(); // skip VAR keyword
    while (scanner_->peekToken()->getType() == TokenType::const_ident) {
        std::vector<std::string> idents;
        ident_list(idents);
        auto token = scanner_->nextToken();
        auto pos = token->getPosition();
        if (token->getType() == TokenType::colon) {
            auto node = type(parent);
            if (node != nullptr) {
                for (auto &&ident : idents) {
                    if (symbols_->isDuplicate(ident)) {
                        logger_->error(token->getPosition(), "duplicate definition: " + ident);
                        continue;
                    }
                    auto variable = std::make_unique<VariableDeclarationNode>(pos, parent, ident, node,
                                                                              symbols_->getLevel(), parent->getOffset());
                    symbols_->insert(ident, variable.get());
                    parent->addVariable(std::move(variable));
                    parent->incOffset(node->getSize());
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

// procedure_declaration = procedure_heading ";" ( procedure_body identifier | "EXTERN" ) ";" .
void Parser::procedure_declaration(BlockNode *parent) {
    logger_->debug("", "procedure_declaration");
    auto proc = procedure_heading(parent);
    auto token = scanner_->peekToken();
    if (token->getType() != TokenType::semicolon) {
        logger_->error(token->getPosition(), "; expected.");
    } else {
        scanner_->nextToken();
    }
    if (scanner_->peekToken()->getType() == TokenType::kw_extern) {
        scanner_->nextToken(); // skip EXTERN keyword
        proc->setExtern(true);
    } else {
        procedure_body(proc.get());
        auto name = ident();
        if (name != proc->getName()) {
            logger_->error(token_->getPosition(), "procedure name mismatch: expected \"" +
                           proc->getName() + "\", found \"" + name + "\".");
        }
    }
    token = scanner_->peekToken();
    if (token->getType() != TokenType::semicolon) {
        logger_->error(token->getPosition(), "; expected.");
    } else {
        scanner_->nextToken();
    }
    symbols_->leaveScope();
    parent->addProcedure(std::move(proc));
}

// procedure_heading = "PROCEDURE" identifier [ formal_parameters ] [ ":" type ] .
std::unique_ptr<ProcedureNode> Parser::procedure_heading(BlockNode *parent) {
    logger_->debug("", "procedure_heading");
    auto token = scanner_->nextToken(); // skip PROCEDURE keyword
    auto proc = std::make_unique<ProcedureNode>(token->getPosition(), parent, ident(), symbols_->getLevel());
    symbols_->insert(proc->getName(), proc.get());
    symbols_->enterScope();
    if (scanner_->peekToken()->getType() == TokenType::lparen) {
        formal_parameters(proc.get());
    }
    if (scanner_->peekToken()->getType() == TokenType::colon) {
        scanner_->nextToken(); // skip colon
        proc->setReturnType(type(proc.get()));
    }
    return proc;
}

// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
void Parser::procedure_body(ProcedureNode *parent) {
    logger_->debug("", "procedure_body");
    declarations(parent);
    if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
        scanner_->nextToken(); // skip BEGIN keyword
        statement_sequence(parent, parent->getStatements());
    }
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::kw_end) {
        scanner_->nextToken();
    } else {
        logger_->error(token->getPosition(), "END expected.");
    }
}

// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")".
void Parser::formal_parameters(ProcedureNode *parent) {
    logger_->debug("", "formal_parameters");
    auto token = scanner_->nextToken(); // skip left parenthesis
    if (token->getType() == TokenType::lparen) {
        TokenType type = scanner_->peekToken()->getType();
        if (type == TokenType::kw_var || type == TokenType::const_ident || type == TokenType::varargs) {
            fp_section(parent);
            while (scanner_->peekToken()->getType() == TokenType::semicolon) {
                token = scanner_->nextToken(); // skip semicolon
                if (parent->hasVarArgs()) {
                    logger_->error(token->getPosition(), "varargs must be last formal parameter.");
                }
                fp_section(parent);
            }
        }
        token = scanner_->nextToken();
        if (token->getType() != TokenType::rparen) {
            logger_->error(token->getPosition(), ") expected.");
        }
    }
}

// fp_section = ( [ "VAR" ] ident_list ":" type | "..." ) .
void Parser::fp_section(ProcedureNode *parent) {
    logger_->debug("", "fp_section");
    if (scanner_->peekToken()->getType() == TokenType::varargs) {
        scanner_->nextToken(); // skip varargs
        parent->setVarArgs(true);
    } else {
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
        auto node = type(parent);
        for (auto ident : idents) {
            auto param = std::make_unique<ParameterNode>(token->getPosition(), parent, ident, node, var, symbols_->getLevel());
            symbols_->insert(ident, param.get());
            parent->addParameter(std::move(param));
        }
    }
}

// statement_sequence = statement { ";" statement } .
void Parser::statement_sequence(BlockNode *parent, StatementSequenceNode *statements) {
    logger_->debug("", "statement_sequence");
    auto token = scanner_->peekToken();
    if (token->getType() == TokenType::kw_end || token->getType() == TokenType::kw_elsif ||
        token->getType() == TokenType::kw_else || token->getType() == TokenType::kw_until) {
        logger_->error(token->getPosition(), "block cannot be empty.");
    } else {
        statements->addStatement(statement(parent));
        while (scanner_->peekToken()->getType() == TokenType::semicolon) {
            scanner_->nextToken(); // skip semicolon
            statements->addStatement(statement(parent));
        }
    }
}

// statement = ( assignment | procedure_call | if_statement | case_statement
//               while_statement | repeat_statement | for_statement | loop_statement
//               with_statement | "EXIT" | "RETURN" [ expression ] ) .
std::unique_ptr<StatementNode> Parser::statement(BlockNode *parent) {
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
            auto lvalue = std::make_unique<ReferenceNode>(pos, var);
            auto context = var->getType();
            token = scanner_->peekToken();
            while (token->getType() == TokenType::period || token->getType() == TokenType::lbrack) {
                context = selector(parent, lvalue.get(), context);
                token = scanner_->peekToken();
            }
            lvalue->setType(context);
            token = scanner_->peekToken();
            if (token->getType() == TokenType::op_becomes) {
                return assignment(parent, std::move(lvalue));
            } else {
                logger_->error(token->getPosition(), "assignment operator := expected.");
            }
        } else if (symbol->getNodeType() == NodeType::procedure) {
            auto call = std::make_unique<ProcedureCallNode>(pos, dynamic_cast<ProcedureNode*>(symbol));
            procedure_call(parent, call.get());
            return call;
        } else if (symbol->getNodeType() == NodeType::constant) {
            logger_->error(pos, "constant cannot be assigned: " + name + ".");
        } else {
            logger_->error(pos, "variable or procedure name expected, found: " + name + ".");
        }
    } else if (token->getType() == TokenType::kw_if) {
        return if_statement(parent);
    } else if (token->getType() == TokenType::kw_loop) {
        return loop_statement(parent);
    } else if (token->getType() == TokenType::kw_while) {
        return while_statement(parent);
    } else if (token->getType() == TokenType::kw_repeat) {
        return repeat_statement(parent);
    } else if (token->getType() == TokenType::kw_for) {
        return for_statement(parent);
    } else if (token->getType() == TokenType::kw_return) {
        token_ = scanner_->nextToken();
        auto expr = expression(parent);
        if (expr) {
            if (!parent->getReturnType()) {
                logger_->error(token_->getPosition(), "procedure cannot return a value.");
            } else if (expr->getType() != parent->getReturnType()) {
                logger_->error(token_->getPosition(),
                               "type mismatch: actual return type does not match declared return type.");
            }
        } else {
            if (!expr && parent->getReturnType()) {
                logger_->error(token_->getPosition(), "function must return value.");
            }
        }
        return std::make_unique<ReturnNode>(token->getPosition(), std::move(expr));
    } else {
        logger_->error(token->getPosition(), "unknown statement: too many semi-colons?");
    }
    resync({ TokenType::semicolon, TokenType::kw_end, TokenType::kw_if, TokenType::kw_elsif, TokenType::kw_else, TokenType::kw_while });
    return nullptr;
}

// assignment = identifier { selector } ":=" expression .
std::unique_ptr<StatementNode> Parser::assignment(BlockNode *parent, std::unique_ptr<ReferenceNode> lvalue) {
    logger_->debug("", "assignment");
    if (lvalue->dereference()->getNodeType() == NodeType::parameter) {
        auto param = dynamic_cast<ParameterNode*>(lvalue->dereference());
        if (!param->isVar()) {
            logger_->error(lvalue->getFilePos(), "cannot assign non-var parameter.");
        }
    }
    auto token = scanner_->nextToken(); // skip assign operator
    auto expr = expression(parent);
    if (expr && lvalue->getType() == expr->getType()) {
        return std::make_unique<AssignmentNode>(lvalue->getFilePos(), std::move(lvalue), std::move(expr));
    }
    logger_->error(token->getPosition(), "illegal assignment: type mismatch.");
    return nullptr;
}

// procedure_call = identifier [ actual_parameters ] .
void Parser::procedure_call(BlockNode *parent, CallNode *call) {
    logger_->debug("", "procedure_call");
    if (scanner_->peekToken()->getType() == TokenType::lparen) {
        actual_parameters(parent, call);
    }
    if (call->getParameterCount() < call->getProcedure()->getParameterCount()) {
        logger_->error(token_->getPosition(), "fewer actual than formal parameters.");
    }
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
std::unique_ptr<StatementNode> Parser::if_statement(BlockNode *parent) {
    logger_->debug("", "if_statement");
    token_ = scanner_->nextToken(); // skip IF keyword
    auto condition = expression(parent);
    if (condition->getType() != BasicTypeNode::BOOLEAN) {
        logger_->error(condition->getFilePos(), "Boolean expression expected.");
    }
    auto statement = std::make_unique<IfThenElseNode>(token_->getPosition(), std::move(condition));
    token_ = scanner_->nextToken();
    if (token_->getType() == TokenType::kw_then) {
        statement_sequence(parent, statement->addThenStatements(token_->getPosition()));
        token_ = scanner_->nextToken();
        while (token_->getType() == TokenType::kw_elsif) {
            condition = expression(parent);
            if (condition->getType() != BasicTypeNode::BOOLEAN) {
                logger_->error(condition->getFilePos(), "Boolean expression expected.");
            }
            token_ = scanner_->nextToken();
            if (token_->getType() == TokenType::kw_then) {
                statement_sequence(parent, statement->addElseIf(token_->getPosition(), std::move(condition)));
            } else {
                logger_->error(token_->getPosition(), "THEN expected, instead of " + to_string(token_->getType()) + ".");
            }
            token_ = scanner_->nextToken();
        }
        if (token_->getType() == TokenType::kw_else) {
            statement_sequence(parent, statement->addElseStatements(token_->getPosition()));
            token_ = scanner_->nextToken();
        }
        if (token_->getType() != TokenType::kw_end) {
            logger_->error(token_->getPosition(), "END expected.");
        }
    } else {
        logger_->error(token_->getPosition(), "THEN expected, instead of " + to_string(token_->getType()) + ".");
    }
    return statement;
}

// loop_statement = "LOOP" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::loop_statement(BlockNode *parent) {
    logger_->debug("", "loop_statement");
    token_ = scanner_->nextToken(); // skip LOOP keyword
    auto statement = std::make_unique<LoopNode>(token_->getPosition());
    statement_sequence(parent, statement->getStatements());
    token_ = scanner_->nextToken();
    if (token_->getType() != TokenType::kw_end) {
        logger_->error(token_->getPosition(), "END expected.");
    }
    return statement;
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::while_statement(BlockNode *parent) {
    logger_->debug("", "while_statement");
    token_ = scanner_->nextToken(); // skip WHILE keyword
    auto condition = expression(parent);
    if (condition->getType() != BasicTypeNode::BOOLEAN) {
        logger_->error(condition->getFilePos(), "Boolean expression expected.");
    }
    auto statement = std::make_unique<WhileLoopNode>(token_->getPosition(), std::move(condition));
    token_ = scanner_->nextToken();
    if (token_->getType() == TokenType::kw_do) {
        statement_sequence(parent, statement->getStatements());
        token_ = scanner_->nextToken();
        if (token_->getType() != TokenType::kw_end) {
            logger_->error(token_->getPosition(), "END expected.");
        }
    } else {
        logger_->error(token_->getPosition(), "DO expected.");
    }
    return statement;
}

// repeat_statement = "REPEAT" statement_sequence "UNTIL" expression .
std::unique_ptr<StatementNode> Parser::repeat_statement(BlockNode *parent) {
    logger_->debug("", "repeat_statement");
    token_ = scanner_->nextToken(); // skip REPEAT keyword
    auto statement = std::make_unique<RepeatLoopNode>(token_->getPosition());
    statement_sequence(parent, statement->getStatements());
    token_ = scanner_->nextToken();
    if (token_->getType() == TokenType::kw_until) {
        auto condition = expression(parent);
        if (condition->getType() != BasicTypeNode::BOOLEAN) {
            logger_->error(condition->getFilePos(), "Boolean expression expected.");
        }
        statement->setCondition(std::move(condition));
    } else {
        logger_->error(token_->getPosition(), "UNTIL expected.");
    }
    return statement;
}

// for_statement = "FOR" ident ":=" expression "TO" expression [ "BY" const_expression ] "DO" statement_sequence "END" .
std::unique_ptr<StatementNode> Parser::for_statement(BlockNode *parent) {
    logger_->debug("", "for_statement");
    token_ = scanner_->nextToken(); // skip FOR keyword
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
        token_ = scanner_->nextToken();
        if (token_->getType()==TokenType::op_becomes) {
            low = expression(parent);
            if (low->getType() != BasicTypeNode::INTEGER) {
                logger_->error(low->getFilePos(), "integer expression expected.");
            }
        }
        else {
            logger_->error(token_->getPosition(), ":= expected");
        }
        token_ = scanner_->nextToken();
        if (token_->getType()==TokenType::kw_to) {
            high = expression(parent);
            if (high->getType() != BasicTypeNode::INTEGER) {
                logger_->error(high->getFilePos(), "integer expression expected.");
            }
        }
        else {
            logger_->error(token_->getPosition(), "TO expected.");
        }
        if (scanner_->peekToken()->getType()==TokenType::kw_by) {
            scanner_->nextToken(); // skip BY keyword
            auto expr = expression(parent);
            if (expr->getType()==BasicTypeNode::INTEGER && expr->isConstant()) {
                step = foldNumber(expr.get());
                if (step == 0) {
                    logger_->error(expr->getFilePos(), "step value cannot be zero.");
                }
            }
            else {
                logger_->error(expr->getFilePos(), "constant integer expression expected.");
            }
        }
        auto statement = std::make_unique<ForLoopNode>(pos, std::move(counter), std::move(low), std::move(high), step);
        token_ = scanner_->nextToken();
        if (token_->getType()==TokenType::kw_do) {
            statement_sequence(parent, statement->getStatements());
        }
        else {
            logger_->error(token_->getPosition(), "DO expected.");
        }
        token_ = scanner_->nextToken();
        if (token_->getType()!=TokenType::kw_end) {
            logger_->error(token_->getPosition(), "END expected.");
        }
        return statement;
    } else {
        logger_->error(pos, name + " cannot be used as a loop counter.");
    }
    return nullptr;
}

// actual_parameters = "(" [ expression { "," expression } ] ")" .
void Parser::actual_parameters(BlockNode *parent, CallNode *call) {
    logger_->debug("", "actual_parameters");
    token_ = scanner_->nextToken(); // skip left parenthesis
    if (scanner_->peekToken()->getType() == TokenType::rparen) {
        token_ = scanner_->nextToken();
        return;
    }
    size_t num = 0;
    auto expr = expression(parent);
    if (expr && checkActualParameter(call->getProcedure(), num, expr.get())) {
        call->addParameter(std::move(expr));
    }
    while (scanner_->peekToken()->getType() == TokenType::comma) {
        token_ = scanner_->nextToken(); // skip comma
        num++;
        expr = expression(parent);
        if (expr && checkActualParameter(call->getProcedure(), num, expr.get())) {
            call->addParameter(std::move(expr));
        }
    }
    token_ = scanner_->nextToken();
    if (token_->getType() != TokenType::rparen) {
        logger_->error(token_->getPosition(), ") expected.");
    }
}

// selector = "." identifier | "[" expression "]" .
TypeNode* Parser::selector(BlockNode *parent, ReferenceNode *ref, const TypeNode *type) {
    logger_->debug("", "selector");
    token_ = scanner_->nextToken();
    if (token_->getType() == TokenType::period) {
        auto name = ident();
        if (type->getNodeType() == NodeType::record_type) {
            auto record = dynamic_cast<const RecordTypeNode*>(type);
            auto field = record->getField(name);
            if (field != nullptr) {
                ref->addSelector(std::make_unique<ReferenceNode>(token_->getPosition(), field));
                return field->getType();
            } else {
                logger_->error(token_->getPosition(), "unknown record field: " + name + ".");
            }
        } else {
            logger_->error(token_->getPosition(), "variable or parameter of RECORD type expected.");
        }
    } else if (token_->getType() == TokenType::lbrack) {
        if (type->getNodeType() == NodeType::array_type) {
            auto array = dynamic_cast<const ArrayTypeNode *>(type);
            auto expr = expression(parent);
            if (expr->getType() != BasicTypeNode::INTEGER) {
                logger_->error(expr->getFilePos(), "integer expression expected.");
            }
            token_ = scanner_->nextToken();
            if (token_->getType() != TokenType::rbrack) {
                logger_->error(token_->getPosition(), "] expected.");
            }
            ref->addSelector(std::move(expr));
            return array->getMemberType();
        } else {
            logger_->error(token_->getPosition(), "variable or parameter of ARRAY type expected.");
        }
    }
    logger_->error(token_->getPosition(), "selector expected.");
    return nullptr;
}

// expression = simple_expression [ ( "=" | "#" | "<" | "<=" | ">" | ">=" ) simple_expression ] .
std::unique_ptr<ExpressionNode> Parser::expression(BlockNode *parent) {
    logger_->debug("", "expression");
    auto lhs = simple_expression(parent);
    TokenType type = scanner_->peekToken()->getType();
    if (type == TokenType::op_eq
        || type == TokenType::op_neq
        || type == TokenType::op_lt
        || type == TokenType::op_leq
        || type == TokenType::op_gt
        || type == TokenType::op_geq) {
        token_ = scanner_->nextToken();
        OperatorType op = token_to_operator(token_->getType());
        auto rhs = simple_expression(parent);
        auto expr = std::make_unique<BinaryExpressionNode>(token_->getPosition(), op, std::move(lhs), std::move(rhs));
        if (expr->getLeftExpression()->isConstant() && expr->getRightExpression()->isConstant()) {
            return fold(expr.get());
        }
        return expr;
    }
    return lhs;
}

// simple_expression = [ "+" | "-" ] term { ( "+" | "-" | OR ) term } .
std::unique_ptr<ExpressionNode> Parser::simple_expression(BlockNode *parent) {
    logger_->debug("", "simple_expression");
    std::unique_ptr<ExpressionNode> expr;
    TokenType type = scanner_->peekToken()->getType();
    if (type == TokenType::op_plus) {
        token_ = scanner_->nextToken();
        expr = term(parent);
    } else if (type == TokenType::op_minus) {
        token_ = scanner_->nextToken();
        expr = std::make_unique<UnaryExpressionNode>(token_->getPosition(), OperatorType::NEG, term(parent));
    } else {
        expr = term(parent);
    }
    type = scanner_->peekToken()->getType();
    while (type == TokenType::op_plus
           || type == TokenType::op_minus
           || type == TokenType::op_or) {
        token_ = scanner_->nextToken();
        OperatorType op = token_to_operator(token_->getType());
        auto temp = std::make_unique<BinaryExpressionNode>(token_->getPosition(), op, std::move(expr), term(parent));
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
std::unique_ptr<ExpressionNode> Parser::term(BlockNode *parent) {
    logger_->debug("", "term");
    auto expr = factor(parent);
    TokenType type = scanner_->peekToken()->getType();
    while (type == TokenType::op_times
           || type == TokenType::op_div
           || type == TokenType::op_mod
           || type == TokenType::op_and) {
        token_ = scanner_->nextToken();
        OperatorType op = token_to_operator(token_->getType());
        auto temp = std::make_unique<BinaryExpressionNode>(token_->getPosition(), op, std::move(expr), factor(parent));
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

// factor = identifier { selector } | function_call | integer | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
std::unique_ptr<ExpressionNode> Parser::factor(BlockNode *parent) {
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
                auto var = dynamic_cast<DeclarationNode *>(node);
                if (var->getLevel() != 1 && parent->getLevel() - var->getLevel() > 1) {
                    logger_->warning(pos, "reference to parent procedure: " + var->getName() + ".");
                }
                auto ref = std::make_unique<ReferenceNode>(pos, var);
                auto context = var->getType();
                token = scanner_->peekToken();
                while (token->getType() == TokenType::period || token->getType() == TokenType::lbrack) {
                    context = selector(parent, ref.get(), context);
                    token = scanner_->peekToken();
                }
                ref->setType(context);
                return ref;
            } else if (node->getNodeType() == NodeType::procedure) {
                auto proc = dynamic_cast<ProcedureNode*>(node);
                if (proc->getReturnType() == nullptr) {
                    logger_->error(pos, "function expected.");
                    return nullptr;
                }
                auto call = std::make_unique<FunctionCallNode>(pos, proc);
                procedure_call(parent, call.get());
                return call;
            } else {
                logger_->error(pos, "constant, parameter, variable or function call expected.");
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
        auto expr = expression(parent);
        token = scanner_->peekToken();
        if (token->getType() == TokenType::rparen) {
            scanner_->nextToken();
        } else {
            logger_->error(token->getPosition(), ") expected.");
        }
        return expr;
    } else if (token->getType() == TokenType::op_not) {
        scanner_->nextToken();
        return std::make_unique<UnaryExpressionNode>(token->getPosition(), OperatorType::NOT, factor(parent));
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
        if (proc->hasVarArgs()) {
            return true;
        }
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
