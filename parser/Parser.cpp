/*
 * Header file of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include <iostream>
#include "Parser.h"

Parser::Parser(Scanner *scanner, Table *symbols, Logger *logger) :
        scanner_(scanner), symbols_(symbols), logger_(logger) {
}

Parser::~Parser() = default;

const ASTNode* Parser::parse() {
    return module();
}

// module = "MODULE" ident ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
const ASTNode* Parser::module() {
    logger_->debug("", "module");
    Token token = scanner_->nextToken();
    if (token.type == TokenType::kw_module) {
        ident();
        token = scanner_->nextToken();
        if (token.type == TokenType::semicolon) {
            declarations();
            token = scanner_->peekToken();
            if (token.type == TokenType::kw_begin) {
                scanner_->nextToken(); // skip BEGIN keyword
                statement_sequence();
            }
            token = scanner_->nextToken();
            if (token.type == TokenType::kw_end) {
                ident();
                token = scanner_->nextToken();
                if (token.type != TokenType::period) {
                    logger_->error(token.pos, ". expected.");
                }
            } else {
                logger_->error(token.pos, "END expected.");
            }
        } else {
            logger_->error(token.pos, "; expected.");
        }
    } else {
        logger_->error(token.pos, "MODULE expected.");
    }
    return nullptr;
}

const std::string Parser::ident() {
    Token token = scanner_->nextToken();
    if (token.type == TokenType::const_ident) {
        std::string ident = scanner_->getIdent();
        logger_->debug("", "ident : " + ident );
        return ident;
    } else {
        logger_->error(token.pos, "identifier expected.");
    }
    return "";
}

// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { ProcedureDeclaration } .
const ASTNode* Parser::declarations() {
    logger_->debug("", "declarations");
    Token token = scanner_->peekToken();
    if (token.type == TokenType::kw_const) {
        const_declarations();
    }
    token = scanner_->peekToken();
    if (token.type == TokenType::kw_type) {
        type_declarations();
    }
    token = scanner_->peekToken();
    if (token.type == TokenType::kw_var) {
        var_declarations();
    }
    token = scanner_->peekToken();
    while (token.type == TokenType::kw_procedure) {
        procedure_declaration();
        token = scanner_->peekToken();
    }
    return nullptr;
}

// const_declarations = "CONST" { ident "=" expression ";" } .
const ASTNode* Parser::const_declarations() {
    logger_->debug("", "const_declarations");
    scanner_->nextToken(); // skip CONST keyword
    Token token = scanner_->peekToken();
    while (token.type == TokenType::const_ident) {
        ident();
        token = scanner_->nextToken();
        if (token.type == TokenType::op_eq) {
            expression();
            token = scanner_->nextToken();
            if (token.type != TokenType::semicolon) {
                logger_->error(token.pos, "; expected.");
            }
        } else {
            logger_->error(token.pos, "= expected.");
        }
        token = scanner_->peekToken();
    }
    return nullptr;
}

// type_declarations =  "TYPE" { ident "=" type ";" } .
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

// procedure_declaration = procedure_heading ";" procedure_body ident ";" .
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
const ASTNode* Parser::expression() {
    logger_->debug("", "expression");
    simple_expression();
    Token token = scanner_->peekToken();
    switch (token.type) {
        case TokenType::op_eq:
        case TokenType::op_neq:
        case TokenType::op_lt:
        case TokenType::op_leq:
        case TokenType::op_gt:
        case TokenType::op_geq:
            token = scanner_->nextToken();
            simple_expression();
            break;
        default:
            // skip others
            break;
    }
    return nullptr;
}

// simple_expression = [ "+" | "-" ] term { ( "+" | "-" | OR ) term } .
const ASTNode* Parser::simple_expression() {
    logger_->debug("", "simple_expression");
    Token token = scanner_->peekToken();
    if (token.type == TokenType::op_plus || token.type == TokenType::op_minus) {
        token = scanner_->nextToken();
    }
    term();
    token = scanner_->peekToken();
    while (token.type == TokenType::op_plus
           || token.type == TokenType::op_minus
           || token.type == TokenType::op_or) {
        token = scanner_->nextToken();
        term();
        token = scanner_->peekToken();
    }
    return nullptr;
}

// term = factor { ( "*" | "DIV" | "MOD" | "&" ) factor } .
const ASTNode* Parser::term() {
    logger_->debug("", "term");
    factor();
    Token token = scanner_->peekToken();
    while (token.type == TokenType::op_mult
           || token.type == TokenType::op_div
           || token.type == TokenType::op_mod
           || token.type == TokenType::op_and) {
        token = scanner_->nextToken();
        factor();
        token = scanner_->peekToken();
    }
    return nullptr;
}

// factor = ident { selector } | number | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
const ASTNode* Parser::factor() {
    logger_->debug("", "factor");
    Token token = scanner_->peekToken();
    if (token.type == TokenType::const_ident) {
        ident();
        token = scanner_->peekToken();
        if (token.type == TokenType::period
            || token.type == TokenType::lbrack) {
            selector();
        }
    } else if (token.type == TokenType::const_number) {
        scanner_->nextToken();
        const int numValue = scanner_->getNumValue();
    } else if (token.type == TokenType::const_string) {
        scanner_->nextToken();
        const std::string strValue = scanner_->getStrValue();
    } else if (token.type == TokenType::const_true) {
        scanner_->nextToken();
    } else if (token.type == TokenType::const_false) {
        scanner_->nextToken();
    } else if (token.type == TokenType::lparen) {
        scanner_->nextToken();
        expression();
        token = scanner_->nextToken();
        if (token.type != TokenType::rparen) {
            logger_->error(token.pos, ") expected.");
        }
    } else if (token.type == TokenType::op_not) {
        scanner_->nextToken();
        factor();
    } else {
        logger_->error(token.pos, "unexpected token.");
    }
    return nullptr;
}

// type = ident | array_type | record_type .
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
        std::make_shared<ArrayTypeSymbol>(0, ts);
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

// ident_list = ident { "," ident } .
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

// procedure_heading = "PROCEDURE" ident [ formal_parameters ] .
const std::shared_ptr<ProcedureSymbol> Parser::procedure_heading() {
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
        ident();
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

// assignment = ident selector ":=" expression .
const ASTNode* Parser::assignment() {
    logger_->debug("", "assignment");
    scanner_->nextToken(); // skip becomes
    expression();
    return nullptr;
}

// procedure_call = ident [ actual_parameters ] .
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
        logger_->error(token.pos, "THEN expected.");
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

// selector = {"." ident | "[" expression "]"}.
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

