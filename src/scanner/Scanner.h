/*
 * Scanner used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H


#include <filesystem>
#include <fstream>
#include <sstream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "Token.h"
#include "LiteralToken.h"
#include "compiler/CompilerConfig.h"
#include "logging/Logger.h"

using std::filesystem::path;
using std::ifstream;
using std::queue;
using std::string;
using std::unique_ptr;
using std::unordered_map;

class Scanner {

private:
    CompilerConfig &config_;
    Logger &logger_;
    const path &path_;
    queue<const Token*> tokens_;
    int lineNo_, charNo_;
    char ch_;
    bool eof_;
    unordered_map<string, TokenType> keywords_;
    ifstream file_;

    void init();
    void read();
    FilePos current() const;
    const Token* scanToken();
    const Token* scanIdent();
    const Token* scanNumber();
    const Token* scanString();
    void scanComment();

public:
    Scanner(CompilerConfig &config, const path &path);
    ~Scanner();
    const Token* peek(bool advance = false);
    unique_ptr<const Token> next();

    static string escape(string str);
    static string unescape(string str);

};


#endif //OBERON0C_SCANNER_H
