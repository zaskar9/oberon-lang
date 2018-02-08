/*
 * Header file of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include <iostream>
#include "Parser.h"
#include "symbol/ParameterSymbol.h"

Parser::Parser(Scanner *scanner, Table *symbols, Logger *logger) : scanner_(scanner), symbols_(symbols), logger_(logger) {

}

Parser::~Parser() = default;

const ASTNode* Parser::parse() {
    return module();
}

// module = "MODULE" ident ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
const ASTNode* Parser::module() {
    std::cout << "module" << std::endl;
    Token token = scanner_->nextToken();
    if (token.type == TokenType::kw_module) {
        ident();
        token = scanner_->nextToken();
        if (token.type == TokenType::semicolon) {
            declarations();
        } else {
            logger_->error(token.pos, "; expected.");
        }
    } else {
        logger_->error(token.pos, "MODULE expected.");
    }
    return nullptr;
}

const std::string Parser::ident() {
    std::cout << "ident";
    Token token = scanner_->nextToken();
    if (token.type == TokenType::const_ident) {
        std::string ident = scanner_->getIdent();
        std::cout << ": " << ident << std::endl;
        return ident;
    } else {
        logger_->error(token.pos, "identifier expected.");
    }
    return nullptr;
}

// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { ProcedureDeclaration } .
const ASTNode* Parser::declarations() {
    std::cout << "declarations" << std::endl;
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
    std::cout << "const_declarations" << std::endl;
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
const ASTNode* Parser::type_declarations() {
    std::cout << "type_declarations" << std::endl;
    scanner_->nextToken(); // skip TYPE keyword
    Token token = scanner_->peekToken();
    while (token.type == TokenType::const_ident) {
        ident();
        token = scanner_->nextToken();
        if (token.type == TokenType::op_eq) {
            type();
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

// var_declarations =  "VAR" { ident_list ":" type ";" } .
const ASTNode* Parser::var_declarations() {
    std::cout << "var_declarations" << std::endl;
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
    std::cout << "procedure_declaration" << std::endl;
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
    std::cout << "expression" << std::endl;
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
    std::cout << "simple_expression" << std::endl;
    Token token = scanner_->peekToken();
    if (token.type == TokenType::op_plus || token.type == TokenType::op_minus) {
        token = scanner_->nextToken();
    }
    term();
    token = scanner_->peekToken();
    while (token.type == TokenType::op_plus || token.type == TokenType::op_minus || token.type == TokenType::op_or) {
        token = scanner_->nextToken();
        term();
        token = scanner_->peekToken();
    }
    return nullptr;
}

// term = factor { ( "*" | "DIV" | "MOD" | "&" ) factor } .
const ASTNode* Parser::term() {
    std::cout << "term" << std::endl;
    factor();
    Token token = scanner_->peekToken();
    while (token.type == TokenType::op_mult || token.type == TokenType::op_div ||
            token.type == TokenType::op_mod || token.type == TokenType::op_and) {
        token = scanner_->nextToken();
        factor();
        token = scanner_->peekToken();
    }
    return nullptr;
}

// factor = ident { selector } | number | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
const ASTNode* Parser::factor() {
    std::cout << "factor" << std::endl;
    Token token = scanner_->peekToken();
    if (token.type == TokenType::const_ident) {
        ident();
        token = scanner_->peekToken();
        if (token.type == TokenType::period || token.type == TokenType::lbrack) {
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
const TypeSymbol* Parser::type() {
    std::cout << "type" << std::endl;
    Token token = scanner_->peekToken();
    if (token.type == TokenType::const_ident) {
        std::string name = ident();
        const Symbol *symbol = symbols_->lookup(name);
    } else if (token.type == TokenType::kw_array) {
        array_type();
    } else if (token.type == TokenType::kw_record) {
        record_type();
    } else {
        logger_->error(token.pos, "unexpected token.");
    }
    return nullptr;
}

// array_type = "ARRAY" expression "OF" type .
const ASTNode* Parser::array_type() {
    std::cout << "array_type" << std::endl;
    scanner_->nextToken(); // skip ARRAY keyword
    expression();
    Token token = scanner_->nextToken();
    if (token.type == TokenType::kw_of) {
        type();
    } else {
        logger_->error(token.pos, "OF expected.");
    }
    return nullptr;
}

// record_type = "RECORD" field_list { ";" field_list } "END" .
const ASTNode* Parser::record_type() {
    std::cout << "record_type" << std::endl;
    scanner_->nextToken(); // skip RECORD keyword
    field_list();
    Token token = scanner_->peekToken();
    while (token.type == TokenType::semicolon) {
        field_list();
        token = scanner_->peekToken();
    }
    token = scanner_->nextToken();
    if (token.type != TokenType::kw_end) {
        logger_->error(token.pos, "END expected.");
    }
    return nullptr;
}

// field_list = ident_list ":" type .
const ASTNode* Parser::field_list() {
    std::cout << "field_list" << std::endl;
    std::list<std::string> idents;
    ident_list(idents);
    Token token = scanner_->nextToken();
    if (token.type == TokenType::colon) {
        type();
    } else {
        logger_->error(token.pos, ": expected.");
    }
    return nullptr;
}

// ident_list = ident { "," ident } .
void Parser::ident_list(std::list<std::string> &idents) {
    std::cout << "ident_list" << std::endl;
    idents.push_back(ident());
    Token token = scanner_->peekToken();
    while (token.type == TokenType::comma) {
        scanner_->nextToken(); // skip comma
        idents.push_back(ident());
        token = scanner_->peekToken();
    }
}

// procedure_heading = "PROCEDURE" ident [ formal_parameters ] .
const ASTNode* Parser::procedure_heading() {
    std::cout << "procedure_heading" << std::endl;
    scanner_->nextToken(); // skip PROCEDURE keyword
    ident();
    Token token = scanner_->peekToken();
    if (token.type == TokenType::lparen) {
        formal_parameters();
    }
    return nullptr;
}

// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
const ASTNode* Parser::procedure_body() {
    std::cout << "procedure_body" << std::endl;
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
const ASTNode* Parser::formal_parameters() {
    std::cout << "formal_parameters" << std::endl;
    Token token = scanner_->nextToken(); // skip left parenthesis
    if (token.type == TokenType::lparen) {
        token = scanner_->peekToken();
        if (token.type == TokenType::kw_var || token.type == TokenType::const_ident) {
            int pos = fp_section(0);
            token = scanner_->peekToken();
            while (token.type == TokenType::semicolon) {
                scanner_->nextToken(); // skip semicolon
                fp_section(pos);
                token = scanner_->peekToken();
            }
        }
        token = scanner_->nextToken();
        if (token.type != TokenType::rparen) {
            logger_->error(token.pos, ") expected.");
        }
    }
    return nullptr;
}

// fp_section = [ "VAR" ] ident_list ":" type .
const int Parser::fp_section(const int start) {
    std::cout << "fp_section" << std::endl;
    int pos = start;
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
    const TypeSymbol *ts = type();
    for (auto const &itr : idents) {
        std::string name = itr;
        auto ps = new ParameterSymbol(itr, ts, var, pos++);
    }
    return pos;
}

// statement_sequence = statement { ";" statement } .
const ASTNode* Parser::statement_sequence() {
    std::cout << "statement_sequence" << std::endl;
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
    std::cout << "statement" << std::endl;
    Token token = scanner_->peekToken();
    if (token.type == TokenType::const_ident) {
        ident();
        token = scanner_->peekToken();
        if (token.type == TokenType::period || token.type == TokenType::lbrack) {
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
    std::cout << "assignment" << std::endl;
    scanner_->nextToken(); // skip becomes
    expression();
    return nullptr;
}

// procedure_call = ident [ actual_parameters ] .
const ASTNode* Parser::procedure_call() {
    std::cout << "procedure_call" << std::endl;
    Token token = scanner_->peekToken();
    if (token.type == TokenType::lparen) {
        actual_parameters();
    }
    return nullptr;
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
const ASTNode* Parser::if_statement() {
    std::cout << "if_statement" << std::endl;
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
    std::cout << "while_statement" << std::endl;
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
    std::cout << "actual_parameters" << std::endl;
    scanner_->nextToken(); // skip left parenthesis
    Token token = scanner_->peekToken();
    if (token.type == TokenType::rparen) {
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
    std::cout << "selector" << std::endl;
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

