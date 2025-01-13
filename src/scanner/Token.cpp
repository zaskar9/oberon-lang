/*
 * Tokens returned by the scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#include "Token.h"

Token::~Token() = default;

TokenType Token::type() const {
    return type_;
}

FilePos Token::start() const {
    return start_;
}

FilePos Token::end() const {
    return end_;
}

void Token::print(std::ostream &stream) const {
    stream << type_;
}

std::ostream& operator<<(std::ostream &stream, const Token &token) {
    token.print(stream);
    return stream;
}

std::ostream& operator<<(std::ostream &stream, const TokenType &type) {
    std::string result;
    switch(type) {
        case TokenType::eof: result = "<eof>"; break;
        case TokenType::undef: result = "<undefined>"; break;
        case TokenType::boolean_literal: result = "BOOLEAN literal"; break;
        case TokenType::byte_literal: result = "BYTE literal"; break;
        case TokenType::char_literal: result = "CHAR literal"; break;
        case TokenType::short_literal: result = "SHORTINT literal"; break;
        case TokenType::int_literal: result = "INTEGER literal"; break;
        case TokenType::long_literal: result = "LONGINT literal"; break;
        case TokenType::float_literal: result = "REAL literal"; break;
        case TokenType::double_literal: result = "LONGREAL literal"; break;
        case TokenType::string_literal: result = "STRING literal"; break;
        case TokenType::const_ident: result = "identifier"; break;
        case TokenType::period: result = "."; break;
        case TokenType::range: result = ".."; break;
        case TokenType::comma: result = ","; break;
        case TokenType::colon: result = ":"; break;
        case TokenType::semicolon: result = ";"; break;
        case TokenType::lparen: result = "("; break;
        case TokenType::rparen: result = ")"; break;
        case TokenType::lbrack: result = "["; break;
        case TokenType::rbrack: result = "]"; break;
        case TokenType::lbrace: result = "{"; break;
        case TokenType::rbrace: result = "}"; break;
        case TokenType::varargs: result = "..."; break;
        case TokenType::pipe: result = "|"; break;
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
        case TokenType::kw_import: result = "IMPORT"; break;
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
        case TokenType::kw_extern: result = "EXTERN"; break;
        case TokenType::kw_return: result = "RETURN"; break;
        case TokenType::kw_nil: result = "NIL"; break;
        default: result = "undefined token"; break;
    }
    stream << result;
    return stream;
}