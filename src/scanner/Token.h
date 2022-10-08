/*
 * Tokens returned by the scanner of the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#ifndef OBERON0C_TOKEN_H
#define OBERON0C_TOKEN_H


#include <ostream>
#include "../logging/Logger.h"

enum class TokenType : char {
    eof, undef,
    boolean_literal, byte_literal, char_literal, integer_literal, longint_literal,
    real_literal, longreal_literal, string_literal,
    const_ident,
    period, comma, colon, semicolon, rparen, lparen, lbrack, rbrack, lbrace, rbrace, caret,
    varargs, pipe,
    op_times, op_div, op_mod, op_plus, op_minus, op_and, op_or, op_not,
    op_eq, op_neq, op_lt, op_gt, op_leq, op_geq, op_becomes,
    kw_module, kw_import, kw_procedure, kw_extern, kw_return, kw_begin, kw_end,
    kw_if, kw_then, kw_else, kw_elsif,
    kw_loop, kw_exit, kw_while, kw_do, kw_repeat, kw_until, kw_for, kw_to, kw_by,
    kw_array, kw_record, kw_const, kw_type, kw_var, kw_of,
    kw_pointer, kw_nil
};

std::ostream& operator<<(std::ostream &stream, const TokenType &type);

class Token {

private:
    TokenType type_;
    FilePos start_, end_;

public:
    explicit Token(const TokenType type, const FilePos &start) :
            Token(type, start, { start.fileName, start.lineNo, start.charNo + 1 }) { };
    explicit Token(const TokenType type, const FilePos &start, const FilePos &end) :
            type_(type), start_(start), end_(end) { };
    virtual ~Token();

    [[nodiscard]] TokenType type() const;
    [[nodiscard]] FilePos start() const;
    [[nodiscard]] FilePos end() const;

    virtual void print(std::ostream &stream) const;
    friend std::ostream& operator<<(std::ostream &stream, const Token &symbol);

};


#endif //OBERON0C_TOKEN_H
