/*
 * Scanner used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H


#include <memory>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include "Token.h"
#include "LiteralToken.h"
#include "../logging/Logger.h"

class Scanner {

private:
    std::string filename_;
    Logger *logger_;
    std::queue<const Token*> tokens_;
    int lineNo_, charNo_;
    std::unordered_map<std::string, TokenType> keywords_;
    std::ifstream file_;
    char ch_{};

    void init();
    void read();
    FilePos current() const;
    const Token* scanToken();
    const Token* scanIdent();
    const Token* scanNumber();
    const Token* scanString();
    void scanComment();

public:
    explicit Scanner(const boost::filesystem::path& path, Logger *logger);
    ~Scanner();
    const Token* peek(bool advance = false);
    std::unique_ptr<const Token> next();

    static std::string escape(std::string str);
    static std::string unescape(std::string str);

};


#endif //OBERON0C_SCANNER_H
