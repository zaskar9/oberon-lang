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
    eof, null, undef,
    const_true, const_false, const_number, const_string, const_ident,
    period, comma, colon, semicolon, rparen, lparen, lbrack, rbrack, varargs,
    op_times, op_div, op_mod, op_plus, op_minus, op_and, op_or, op_not,
    op_eq, op_neq, op_lt, op_gt, op_leq, op_geq, op_becomes,
    kw_module, kw_procedure, kw_begin, kw_end, kw_if, kw_then, kw_else, kw_elsif,
    kw_loop, kw_exit, kw_while, kw_do, kw_repeat, kw_until, kw_for, kw_to, kw_by,
    kw_array, kw_record, kw_const, kw_type, kw_var, kw_of, kw_declare, kw_extern,
    kw_return
};

std::ostream& operator<<(std::ostream &stream, const TokenType &type);

class Token {

private:
    TokenType type_;
    FilePos pos_;

public:
    explicit Token(const TokenType type, const FilePos &pos) : type_(type), pos_(pos) { };
    virtual ~Token();

    [[nodiscard]] TokenType getType() const;
    [[nodiscard]] FilePos getPosition() const;

    virtual void print(std::ostream &stream) const;
    friend std::ostream& operator<<(std::ostream &stream, const Token &symbol);

};


#endif //OBERON0C_TOKEN_H
