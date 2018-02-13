/*
 * Header of the scanner class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H

#include <string>
#include <fstream>
#include <unordered_map>
#include "../util/Logger.h"

enum class TokenType : char {
    eof, null,
    const_true, const_false, const_number, const_string, const_ident,
    period, comma, colon, semicolon, rparen, lparen, lbrack, rbrack,
    op_mult, op_div, op_mod, op_plus, op_minus, op_and, op_or, op_not,
    op_eq, op_neq, op_lt, op_gt, op_leq, op_geq, op_becomes,
    kw_module, kw_procedure, kw_begin, kw_end, kw_if, kw_then, kw_else, kw_elsif, kw_while, kw_do,
    kw_array, kw_record, kw_const, kw_type, kw_var, kw_of
};

std::ostream& operator<<(std::ostream &stream, const TokenType &type);

struct Token {
    TokenType type;
    FilePos pos;
};

class Scanner {

private:
    std::string filename_;
    std::ifstream file_;
    std::unordered_map<std::string, TokenType> keywords_;
    Logger *logger_;
    Token token_;
    char ch_;
    int lineNo_, charNo_, numValue_;
    std::string strValue_, ident_;

    void initTable();
    void read();
    const FilePos getPosition() const;
    void comment();
    const Token ident();
    const int number();
    const std::string string();

public:
    explicit Scanner(const std::string &filename, Logger *logger);
    ~Scanner();
    const Token nextToken();
    const Token peekToken();
    const int getNumValue() const;
    const std::string getStrValue() const;
    const std::string getIdent() const;

};

#endif //OBERON0C_SCANNER_H
