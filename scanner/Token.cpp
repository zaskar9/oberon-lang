/*
 * Tokens returned by the scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#include "Token.h"

Token::~Token() = default;

TokenType Token::getType() const {
    return type_;
}

FilePos Token::getPosition() const {
    return pos_;
}

void Token::print(std::ostream &stream) const {
    stream << type_;
}

std::ostream& operator<<(std::ostream &stream, const Token &symbol) {
    symbol.print(stream);
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const TokenType &type) {
    std::string result;
    switch(type) {
        case TokenType::eof: result = "EOF"; break;
        case TokenType::null: result = "NULL"; break;
        case TokenType::undef: result = "UNDEFINED"; break;
        case TokenType::boolean_literal: result = "boolean literal"; break;
        case TokenType::byte_literal: result = "byte literal"; break;
        case TokenType::char_literal: result = "char literal"; break;
        case TokenType::integer_literal: result = "integer literal"; break;
        case TokenType::longint_literal: result = "longint literal"; break;
        case TokenType::real_literal: result = "real literal"; break;
        case TokenType::longreal_literal: result = "longreal literal"; break;
        case TokenType::string_literal: result = "string literal"; break;
        case TokenType::const_ident: result = "identifier"; break;
        case TokenType::period: result = "."; break;
        case TokenType::comma: result = ","; break;
        case TokenType::colon: result = ":"; break;
        case TokenType::semicolon: result = ";"; break;
        case TokenType::lparen: result = "("; break;
        case TokenType::rparen: result = ")"; break;
        case TokenType::lbrack: result = "["; break;
        case TokenType::rbrack: result = "]"; break;
        case TokenType::varargs: result = "..."; break;
        case TokenType::op_times: result = "*"; break;
        case TokenType::op_div: result = "DIV"; break;
        case TokenType::op_mod: result = "MOD"; break;
        case TokenType::op_plus: result = "+"; break;
        case TokenType::op_minus: result = "-"; break;
        case TokenType::op_and: result = "&"; break;
        case TokenType::op_or: result = "OR"; break;
        case TokenType::op_not: result = "~"; break;
        case TokenType::op_eq: result = "="; break;
        case TokenType::op_neq: result = "#"; break;
        case TokenType::op_lt: result = "<"; break;
        case TokenType::op_gt: result = ">"; break;
        case TokenType::op_leq: result = "<="; break;
        case TokenType::op_geq: result = ">="; break;
        case TokenType::op_becomes: result = ":="; break;
        case TokenType::kw_module: result = "MODULE"; break;
        case TokenType::kw_procedure: result = "PROCEDURE"; break;
        case TokenType::kw_begin: result = "BEGIN"; break;
        case TokenType::kw_end: result = "END"; break;
        case TokenType::kw_if: result = "IF"; break;
        case TokenType::kw_then: result = "THEN"; break;
        case TokenType::kw_else: result = "ELSE"; break;
        case TokenType::kw_elsif: result = "ELSIF"; break;
        case TokenType::kw_loop: result = "LOOP"; break;
        case TokenType::kw_exit: result = "EXIT"; break;
        case TokenType::kw_while: result = "WHILE"; break;
        case TokenType::kw_do: result = "DO"; break;
        case TokenType::kw_repeat: result = "REPEAT"; break;
        case TokenType::kw_until: result = "UNTIL"; break;
        case TokenType::kw_for: result = "FOR"; break;
        case TokenType::kw_to: result = "TO"; break;
        case TokenType::kw_by: result = "BY"; break;
        case TokenType::kw_array: result = "ARRAY"; break;
        case TokenType::kw_record: result = "RECORD"; break;
        case TokenType::kw_const: result = "CONST"; break;
        case TokenType::kw_type: result = "TYPE"; break;
        case TokenType::kw_var: result = "VAR"; break;
        case TokenType::kw_of: result = "OF"; break;
        case TokenType::kw_declare: result = "DECLARE"; break;
        case TokenType::kw_extern: result = "EXTERN"; break;
        case TokenType::kw_return: result = "RETURN"; break;
    }
    stream << result;
    return stream;
}