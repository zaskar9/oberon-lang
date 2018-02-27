/*
 * Header of the scanner class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H

#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "Token.h"
#include "../util/Logger.h"


class Scanner {

private:
    std::string filename_;
    const Logger *logger_;
    const Token *token_;
    int lineNo_, charNo_;
    std::unordered_map<std::string, TokenType> keywords_;
    std::ifstream file_;
    char ch_;

    void initTable();
    void read();
    const FilePos getPosition() const;
    const Token* next();
    const Token* ident();
    const int number();
    const std::string string();
    void comment();

public:
    explicit Scanner(const std::string &filename, const Logger *logger);
    ~Scanner();
    const Token* peekToken();
    std::unique_ptr<const Token> nextToken();

};

#endif //OBERON0C_SCANNER_H
