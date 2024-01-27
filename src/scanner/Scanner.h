/*
 * Scanner used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H


#include "Token.h"
#include "LiteralToken.h"
#include "logging/Logger.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <sstream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

namespace fs = boost::filesystem;

class Scanner {

private:
    const fs::path &path_;
    Logger *logger_;
    std::queue<const Token*> tokens_;
    int lineNo_, charNo_;
    char ch_;
    bool eof_;
    std::unordered_map<std::string, TokenType> keywords_;
    std::ifstream file_;

    void init();
    void read();
    FilePos current() const;
    const Token* scanToken();
    const Token* scanIdent();
    const Token* scanNumber();
    const Token* scanString();
    void scanComment();

public:
    Scanner(const fs::path &path, Logger *logger);
    ~Scanner();
    const Token* peek(bool advance = false);
    std::unique_ptr<const Token> next();

    static std::string escape(std::string str);
    static std::string unescape(std::string str);

};


#endif //OBERON0C_SCANNER_H
