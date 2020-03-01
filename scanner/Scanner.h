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
#include <queue>
#include <boost/filesystem.hpp>
#include "Token.h"
#include "../util/Logger.h"


class Scanner {

private:
    std::string filename_;
    Logger *logger_;
    std::queue<const Token*> tokens_;
    int lineNo_, charNo_;
    std::unordered_map<std::string, TokenType> keywords_;
    std::ifstream file_;
    char ch_;

    void initTable();
    void read();
    const FilePos getPosition() const;
    const Token* next();
    const Token* ident();
    int number();
    const std::string string();
    void comment();

public:
    explicit Scanner(boost::filesystem::path path, Logger *logger);
    ~Scanner();
    const Token* peekToken();
    std::unique_ptr<const Token> nextToken();

    static std::string escape(std::string str);
    static std::string unescape(std::string str);

};

#endif //OBERON0C_SCANNER_H
