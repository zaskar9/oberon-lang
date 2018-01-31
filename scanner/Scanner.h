//
// Created by Michael Grossniklaus on 12/15/17.
//

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H

#include <fstream>
#include <unordered_map>
#include <string>

enum class Token : char
{
    eof,
    const_null, const_true, const_false, const_number, const_ident,
    period, comma, colon, semicolon, rparen, lparen, lbrack, rbrack,
    op_mult, op_div, op_mod, op_plus, op_minus, op_and, op_or, op_not,
    op_eq, op_neq, op_lt, op_gt, op_leq, op_geq, op_assign,
    kw_module, kw_procedure, kw_begin, kw_end, kw_if, kw_then, kw_else, kw_elsif, kw_while, kw_do,
    kw_array, kw_record, kw_const, kw_type, kw_var, kw_of
};

class Scanner
{

private:
    const int maxIdentifierLen = 32;
    std::ifstream _file;
    std::unordered_map<std::string, Token> _keywords;
    std::string _ident = "";
    int _value = -1;
    char _ch;
    int _lineNo, _charNo;

    void initTable();
    void read();
    void comment();
    const Token ident();
    void number();

public:
    explicit Scanner(const std::string& filename);
    ~Scanner();
    const Token nextToken();
    const int getCharNo() const;
    const int getLineNo() const;

};

#endif //OBERON0C_SCANNER_H
