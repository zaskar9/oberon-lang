/*
 * Tokens returned by the scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#ifndef OBERON0C_TOKEN_H
#define OBERON0C_TOKEN_H


#include <ostream>
#include "../util/Logger.h"

enum class TokenType : char {
    eof, nil, undef,
    boolean_literal, byte_literal, char_literal, integer_literal, longint_literal,
    real_literal, longreal_literal, string_literal,
    const_ident,
    period, comma, colon, semicolon, rparen, lparen, lbrack, rbrack, lbrace, rbrace,
    varargs, pipe,
    op_times, op_div, op_mod, op_plus, op_minus, op_and, op_or, op_not,
    op_eq, op_neq, op_lt, op_gt, op_leq, op_geq, op_becomes,
    kw_module, kw_procedure, kw_begin, kw_end, kw_if, kw_then, kw_else, kw_elsif,
    kw_loop, kw_exit, kw_while, kw_do, kw_repeat, kw_until, kw_for, kw_to, kw_by,
    kw_array, kw_record, kw_const, kw_type, kw_var, kw_of, kw_extern, kw_return
};

std::ostream& operator<<(std::ostream &stream, const TokenType &type);

class Token {

private:
    TokenType type_;
    FilePos pos_;

public:
    explicit Token(const TokenType type, const FilePos &pos) : type_(type), pos_(pos) { };
    virtual ~Token();

    [[nodiscard]] TokenType type() const;
    [[nodiscard]] FilePos pos() const;

    virtual void print(std::ostream &stream) const;
    friend std::ostream& operator<<(std::ostream &stream, const Token &symbol);

};


#endif //OBERON0C_TOKEN_H
