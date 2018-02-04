//
// Created by Michael Grossniklaus on 12/15/17.
//

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H

#include <fstream>
#include <unordered_map>
#include <string>

enum class Token : char {
    eof, null,
    const_true, const_false, const_number, const_ident,
    period, comma, colon, semicolon, rparen, lparen, lbrack, rbrack,
    op_mult, op_div, op_mod, op_plus, op_minus, op_and, op_or, op_not,
    op_eq, op_neq, op_lt, op_gt, op_leq, op_geq, op_becomes,
    kw_module, kw_procedure, kw_begin, kw_end, kw_if, kw_then, kw_else, kw_elsif, kw_while, kw_do,
    kw_array, kw_record, kw_const, kw_type, kw_var, kw_of
};

class Scanner {

private:
    const int maxIdentifierLen = 32;
    std::string _filename;
    std::ifstream _file;
    std::unordered_map<std::string, Token> _keywords;
    std::string _ident;
    int _value;
    char _ch;
    int _lineNo, _charNo;
    Token _token;

    void initTable();
    void read();
    void logError(const std::string& msg);
    void comment();
    const Token ident();
    void number();

public:
    explicit Scanner(const std::string& filename);
    ~Scanner();
    const Token nextToken();
    const Token peekToken();
    const int getCharNo() const;
    const int getLineNo() const;
    const int getValue() const;
    const std::string getIdent() const;
    const std::string getFileName() const;

};

#endif //OBERON0C_SCANNER_H
