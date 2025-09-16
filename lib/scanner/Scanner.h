/*
 * Scanner used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H


#include <filesystem>
#include <fstream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "global.h"
#include "Logger.h"
#include "Token.h"

using std::filesystem::path;
using std::ifstream;
using std::queue;
using std::streampos;
using std::string;
using std::unique_ptr;
using std::unordered_map;

class Scanner {

public:
    Scanner(Logger &, const path &);
    ~Scanner();
    const Token* peek(bool = false);
    unique_ptr<const Token> next();
    void seek(const FilePos &);

    static string escape(string str);
    static string unescape(string str);

private:
    Logger &logger_;
    const path &path_;
    queue<unique_ptr<const Token>> tokens_;
    int lineNo_, charNo_;
    char ch_;
    bool eof_;
    unordered_map<string, TokenType> keywords_;
    ifstream file_;

    void init();
    void read();
    FilePos current();
    unique_ptr<const Token> scanToken();
    unique_ptr<const Token> scanIdent();
    unique_ptr<const Token> scanNumber();
    // unique_ptr<const Token> scanCharacter();
    unique_ptr<const Token> scanString();
    void scanComment(const FilePos &);

};


#endif //OBERON0C_SCANNER_H
