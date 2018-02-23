/*
 * Header file of the class for the token used by parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/23/18.
 */

#ifndef OBERON0C_TOKEN_H
#define OBERON0C_TOKEN_H


#include <ostream>
#include "../util/Logger.h"

enum class TokenType : char {
    eof, null,
    const_true, const_false, const_number, const_string, const_ident,
    period, comma, colon, semicolon, rparen, lparen, lbrack, rbrack,
    op_times, op_div, op_mod, op_plus, op_minus, op_and, op_or, op_not,
    op_eq, op_neq, op_lt, op_gt, op_leq, op_geq, op_becomes,
    kw_module, kw_procedure, kw_begin, kw_end, kw_if, kw_then, kw_else, kw_elsif, kw_while, kw_do,
    kw_array, kw_record, kw_const, kw_type, kw_var, kw_of
};

std::ostream& operator<<(std::ostream &stream, const TokenType &type);

class Token {

private:
    TokenType type_;
    FilePos pos_;

public:
    Token(TokenType type, FilePos pos);
    virtual ~Token();

    const TokenType getType() const;
    const FilePos getPosition() const;

    virtual void print(std::ostream &stream) const;
    friend std::ostream& operator<<(std::ostream &stream, const Token &symbol);

};


#endif //OBERON0C_TOKEN_H
