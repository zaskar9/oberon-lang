/*
 * Header file of the parser class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include <iostream>
#include "Parser.h"

Parser::Parser(Scanner* scanner) {
    scanner_ = scanner;
}

Parser::~Parser() {
    // TODO
}

void Parser::logError(const std::string &msg) {
    std::cerr << scanner_->getFileName() << ":" << scanner_->getLineNo() << ":" << scanner_->getCharNo() << ": error: " << msg << std::endl;
}

const ASTNode* Parser::parse() {
    return module();
}

// module = "MODULE" ident ";" declarations [ "BEGIN" statement_sequence ] "END" ident "." .
const ASTNode* Parser::module() {
    std::cout << "module" << std::endl;
    if (scanner_->nextToken() == Token::kw_module) {
        ident();
        if (scanner_->nextToken() == Token::semicolon) {
            declarations();
        } else {
            logError("; expected.");
        }
    } else {
        logError("MODULE expected.");
    }
    return NULL;
}

const ASTNode* Parser::ident() {
    std::cout << "ident";
    if (scanner_->nextToken() == Token::const_ident) {
        std::string ident = scanner_->getIdent();
        std::cout << ": " << ident;
    } else {
        logError("identifier expected.");
    }
    std::cout << std::endl;
    return NULL;
}

// declarations = [ const_declarations ] [ type_declarations ] [ var_declarations ] { ProcedureDeclaration } .
const ASTNode* Parser::declarations() {
    std::cout << "declarations" << std::endl;
    if (scanner_->peekToken() == Token::kw_const) {
        const_declarations();
    }
    if (scanner_->peekToken() == Token::kw_type) {
        type_declarations();
    }
    if (scanner_->peekToken() == Token::kw_var) {
        var_declarations();
    }
    while (scanner_->peekToken() == Token::kw_procedure) {
        procedure_declaration();
    }
    return NULL;
}

// const_declarations = "CONST" { ident "=" expression ";" } .
const ASTNode* Parser::const_declarations() {
    std::cout << "const_declarations" << std::endl;
    scanner_->nextToken();
    while (scanner_->peekToken() == Token::const_ident) {
        ident();
        if (scanner_->nextToken() == Token::op_eq) {
            expression();
            if (scanner_->nextToken() != Token::semicolon) {
                logError("; expected.");
            }
        } else {
            logError("= expected.");
        }
    }
    return NULL;
}

// type_declarations =  "TYPE" { ident "=" type ";" } .
const ASTNode* Parser::type_declarations() {
    std::cout << "type_declarations" << std::endl;
    scanner_->nextToken();
    while (scanner_->peekToken() == Token::const_ident) {
        ident();
        if (scanner_->nextToken() == Token::op_eq) {
            type();
            if (scanner_->nextToken() != Token::semicolon) {
                logError("; expected.");
            }
        } else {
            logError("= expected.");
        }
    }
    return NULL;
}

// var_declarations =  "VAR" { ident_list ":" type ";" } .
const ASTNode* Parser::var_declarations() {
    std::cout << "var_declarations" << std::endl;
    scanner_->nextToken();
    while (scanner_->peekToken() == Token::const_ident) {
        ident_list();
        if (scanner_->nextToken() == Token::colon) {
            type();
            if (scanner_->nextToken() != Token::semicolon) {
                logError("; expected.");
            }
        } else {
            logError(": expected.");
        }
    }
    return NULL;
}

// procedure_declaration = procedure_heading ";" procedure_body ident ";" .
const ASTNode* Parser::procedure_declaration() {
    std::cout << "procedure_declaration" << std::endl;
    procedure_heading();
    if (scanner_->nextToken() != Token::semicolon) {
        logError("; semicolon.");
    }
    procedure_body();
    ident();
    if (scanner_->nextToken() != Token::semicolon) {
        logError("; semicolon.");
    }
    return NULL;
}

// expression = simple_expression [ ( "=" | "#" | "<" | "<=" | ">" | ">=" ) simple_expression ] .
const ASTNode* Parser::expression() {
    std::cout << "expression" << std::endl;
    simple_expression();
    switch (scanner_->peekToken()) {
        case Token::op_eq:
        case Token::op_neq:
        case Token::op_lt:
        case Token::op_leq:
        case Token::op_gt:
        case Token::op_geq:
            scanner_->nextToken();
            simple_expression();
            break;
        default:
            // skip others
            break;
    }
    return NULL;
}

// simple_expression = [ "+" | "-" ] term { ( "+" | "-" | OR ) term } .
const ASTNode* Parser::simple_expression() {
    std::cout << "simple_expression" << std::endl;
    Token token = scanner_->peekToken();
    if (token == Token::op_plus || token == Token::op_minus) {
        scanner_->nextToken();
    }
    term();
    token = scanner_->peekToken();
    while (token == Token::op_plus || token == Token::op_minus || token == Token::op_or) {
        scanner_->nextToken();
        term();
        token = scanner_->peekToken();
    }
    return NULL;
}

// term = factor { ( "*" | "DIV" | "MOD" | "&" ) factor } .
const ASTNode* Parser::term() {
    std::cout << "term" << std::endl;
    factor();
    Token token = scanner_->peekToken();
    while (token == Token::op_mult || token == Token::op_div || token == Token::op_mod || token == Token::op_and) {
        scanner_->nextToken();
        factor();
        token = scanner_->peekToken();
    }
    return NULL;
}

// factor = ident { selector } | number | string | "TRUE" | "FALSE" | "(" expression ")" | "~" factor .
const ASTNode* Parser::factor() {
    std::cout << "factor" << std::endl;
    Token token = scanner_->peekToken();
    if (token == Token::const_ident) {
        ident();
        token = scanner_->peekToken();
        if (token == Token::period || token == Token::lbrack) {
            selector();
        }
    } else if (token == Token::const_number) {
        scanner_->nextToken();
        const int numValue = scanner_->getNumValue();
    } else if (token == Token::const_string) {
        scanner_->nextToken();
        const std::string strValue = scanner_->getStrValue();
    } else if (token == Token::const_true) {
        scanner_->nextToken();
    } else if (token == Token::const_false) {
        scanner_->nextToken();
    } else if (token == Token::lparen) {
        scanner_->nextToken();
        expression();
        if (scanner_->nextToken() != Token::rparen)
        {
            logError(") expected.");
        }
    } else if (token == Token::op_not) {
        scanner_->nextToken();
        factor();
    } else {
        logError("unexpected token.");
    }
    return NULL;
}

// type = ident | array_type | record_type .
const ASTNode* Parser::type() {
    std::cout << "type" << std::endl;
    Token token = scanner_->peekToken();
    if (token == Token::const_ident) {
        ident();
    } else if (token == Token::kw_array) {
        array_type();
    } else if (token == Token::kw_record) {
        record_type();
    } else {
        logError("unecpected token.");
    }
    return NULL;
}

// array_type = "ARRAY" expression "OF" type .
const ASTNode* Parser::array_type() {
    std::cout << "array_type" << std::endl;
    scanner_->nextToken();
    expression();
    if (scanner_->nextToken() == Token::kw_of) {
        type();
    } else {
        logError("OF expected.");
    }
    return NULL;
}

// record_type = "RECORD" field_list { ";" field_list } "END" .
const ASTNode* Parser::record_type() {
    std::cout << "record_type" << std::endl;
    scanner_->nextToken();
    field_list();
    while (scanner_->peekToken() == Token::semicolon) {
        field_list();
    }
    if (scanner_->nextToken() != Token::kw_end) {
        logError("END expected.");
    }
    return NULL;
}

// field_list = ident_list ":" type .
const ASTNode* Parser::field_list() {
    std::cout << "field_list" << std::endl;
    ident_list();
    if (scanner_->nextToken() == Token::colon) {
        type();
    } else {
        logError(": expected.");
    }
    return NULL;
}

// ident_list = ident { "," ident } .
const ASTNode* Parser::ident_list() {
    std::cout << "ident_list" << std::endl;
    ident();
    while (scanner_->peekToken() == Token::comma) {
        scanner_->nextToken();
        ident();
    }
    return NULL;
}

// procedure_heading = "PROCEDURE" ident [ formal_parameters ] .
const ASTNode* Parser::procedure_heading() {
    std::cout << "procedure_heading" << std::endl;
    scanner_->nextToken();
    ident();
    if (scanner_->peekToken() == Token::lparen) {
        formal_parameters();
    }
    return NULL;
}

// procedure_body = declarations [ "BEGIN" statement_sequence ] "END" .
const ASTNode* Parser::procedure_body() {
    std::cout << "procedure_body" << std::endl;
    declarations();
    if (scanner_->peekToken() == Token::kw_begin) {
        scanner_->nextToken();
        statement_sequence();
    }
    if (scanner_->nextToken() != Token::kw_end) {
        logError("END expected.");
    }
    return NULL;
}

// formal_parameters = "(" [ fp_section { ";" fp_section } ] ")".
const ASTNode* Parser::formal_parameters() {
    std::cout << "formal_parameters" << std::endl;
    if (scanner_->nextToken() == Token::lparen) {
        Token token = scanner_->peekToken();
        if (token == Token::kw_var || token == Token::const_ident) {
            fp_section();
            while (scanner_->peekToken() == Token::semicolon) {
                scanner_->nextToken();
                fp_section();
            }
        }
        if (scanner_->nextToken() != Token::rparen) {
            logError(") expected.");
        }
    }
    return NULL;
}

// fp_section = [ "VAR" ] ident_list ":" type .
const ASTNode* Parser::fp_section() {
    std::cout << "fp_section" << std::endl;
    Token token = scanner_->peekToken();
    if (token == Token::kw_var) {
        scanner_->nextToken();
    }
    ident_list();
    if (scanner_->nextToken() != Token::colon) {
        logError(": expected.");
    }
    type();
    return NULL;
}

// statement_sequence = statement { ";" statement } .
const ASTNode* Parser::statement_sequence() {
    std::cout << "statement_sequence" << std::endl;
    statement();
    while (scanner_->peekToken() == Token::semicolon) {
        scanner_->nextToken();
        statement();
    }
    return NULL;
}

// statement = [ assignment | procedure_call | if_statement | while_statement ] .
const ASTNode* Parser::statement() {
    std::cout << "statement" << std::endl;
    Token token = scanner_->peekToken();
    if (token == Token::const_ident) {
        ident();
        token = scanner_->peekToken();
        if (token == Token::period || token == Token::lbrack) {
            selector();
        }
        if (scanner_->peekToken() == Token::op_becomes) {
            assignment();
        } else {
            procedure_call();
        }
    } else if (token == Token::kw_if) {
        if_statement();
    } else if (token == Token::kw_while) {
        while_statement();
    } else {
        logError("unknown statement.");
    }
    return NULL;
}

// assignment = ident selector ":=" expression .
const ASTNode* Parser::assignment() {
    std::cout << "assignment" << std::endl;
    scanner_->nextToken();
    expression();
    return NULL;
}

// procedure_call = ident [ actual_parameters ] .
const ASTNode* Parser::procedure_call() {
    std::cout << "procedure_call" << std::endl;
    if (scanner_->peekToken() == Token::lparen) {
        actual_parameters();
    }
    return NULL;
}

// if_statement = "IF" expression "THEN" statement_sequence { "ELSIF" expression "THEN" statement_sequence } [ "ELSE" statement_sequence ] "END" .
const ASTNode* Parser::if_statement() {
    std::cout << "if_statement" << std::endl;
    scanner_->nextToken();
    expression();
    if (scanner_->nextToken() == Token::kw_then) {
        statement_sequence();
        while (scanner_->peekToken() == Token::kw_elsif) {
            scanner_->nextToken();
            statement_sequence();
        }
        if (scanner_->peekToken() == Token::kw_else) {
            statement_sequence();
        }
        if (scanner_->nextToken() != Token::kw_end) {
            logError("END expected.");
        }
    } else {
        logError("THEN expected.");
    }
    return NULL;
}

// while_statement = "WHILE" expression "DO" statement_sequence "END" .
const ASTNode* Parser::while_statement() {
    std::cout << "while_statement" << std::endl;
    scanner_->nextToken();
    expression();
    if (scanner_->nextToken() == Token::kw_do) {
        statement_sequence();
        if (scanner_->nextToken() != Token::kw_end) {
            logError("END expected.");
        }
    } else {
        logError("DO expected.");
    }
    return NULL;
}

// actual_parameters = "(" [ expression { "," expression } ] ")" .
const ASTNode* Parser::actual_parameters() {
    std::cout << "actual_parameters" << std::endl;
    scanner_->nextToken();
    if (scanner_->peekToken() == Token::rparen) {
        return NULL;
    }
    expression();
    while (scanner_->peekToken() == Token::comma) {
        scanner_->nextToken();
        expression();
    }
    if (scanner_->nextToken() != Token::rparen) {
        logError(") expected.");
    }
    return NULL;
}

// selector = {"." ident | "[" expression "]"}.
const ASTNode* Parser::selector() {
    std::cout << "selector" << std::endl;
    Token token = scanner_->nextToken();
    if (token == Token::period) {
        ident();
    } else if (token == Token::lbrack) {
        expression();
        if (scanner_->nextToken() != Token::rbrack) {
            logError("] expected.");
        }
    }
    return NULL;
}

