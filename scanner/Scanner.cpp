/*
 * Implementation of the scanner class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#include "Scanner.h"
#include <iostream>
#include <sstream>

Scanner::Scanner(const std::string& filename) : ident_(), numValue_(-1), lineNo_(1), charNo_(0), token_(Token::null) {
    filename_ = filename;
    file_.open(filename_, std::ios::in);
    if (!file_.is_open()) {
        std::cout << "Cannot open file." << std::endl;
        exit(1);
    }
    read();
    initTable();
}

Scanner::~Scanner() {
    if (file_.is_open()) {
        file_.close();
    }
}

const int Scanner::getCharNo() const {
    return charNo_;
}

const int Scanner::getLineNo() const {
    return lineNo_;
}

const int Scanner::getNumValue() const {
    return numValue_;
}

const std::string Scanner::getStrValue() const {
    return strValue_;
}

const std::string Scanner::getIdent() const {
    return ident_;
}

const std::string Scanner::getFileName() const {
    return filename_;
}

void Scanner::initTable() {
    keywords_ = { { "DIV", Token::op_div }, { "MOD", Token::op_mod }, { "OR", Token::op_or },
                  { "MODULE", Token::kw_module }, { "PROCEDURE", Token::kw_procedure },
                  { "BEGIN", Token::kw_begin }, { "END", Token::kw_end },
                  { "WHILE", Token::kw_while }, { "DO", Token::kw_do},
                  { "IF", Token::kw_if }, { "THEN", Token::kw_then },
                  { "ELSE", Token::kw_else }, { "ELSIF", Token::kw_elsif },
                  { "VAR", Token::kw_var }, { "CONST", Token::kw_const },
                  { "TYPE", Token::kw_type }, { "ARRAY", Token::kw_array },
                  { "RECORD", Token::kw_record }, { "OF", Token::kw_of },
                  { "TRUE", Token::const_true }, { "FALSE", Token::const_false } };
}

const Token Scanner::nextToken() {
    Token token;
    if (token_ != Token::null) {
        token = token_;
        token_ = Token::null;
        return token;
    }
    numValue_ = -1;
    ident_ = "";
    // Skip whitespace
    while ((ch_ != -1) && (ch_ <= ' ')) {
        read();
    }
    if (ch_ != -1) {
        if (((ch_ >= 'A') && (ch_ <= 'Z')) || ((ch_ >= 'a') && (ch_ <= 'z'))) {
            // Scan identifier
            token = ident();
        } else if ((ch_ >= '0') && (ch_ <= '9')) {
            // Scan number
            token = Token::const_number;
            numValue_ = number();
        } else {
            switch (ch_) {
                case '&':
                    token = Token::op_and;
                    read();
                    break;
                case '*':
                    token = Token::op_mult;
                    read();
                    break;
                case '+':
                    token = Token::op_plus;
                    read();
                    break;
                case '-':
                    token = Token::op_minus;
                    read();
                    break;
                case '=':
                    token = Token::op_eq;
                    read();
                    break;
                case '#':
                    token = Token::op_neq;
                    read();
                    break;
                case '<':
                    read();
                    if (ch_ == '=') {
                        token = Token::op_leq;
                        read();
                    } else {
                        token = Token::op_lt;
                    }
                    break;
                case '>':
                    read();
                    if (ch_ == '=') {
                        token = Token::op_geq;
                        read();
                    } else {
                        token = Token::op_gt;
                    }
                    break;
                case ';':
                    token = Token::semicolon;
                    read();
                    break;
                case ',':
                    token = Token::comma;
                    read();
                    break;
                case ':':
                    read();
                    if (ch_ == '=') {
                        token = Token::op_becomes;
                        read();
                    } else {
                        token = Token::colon;
                    }
                    break;
                case '.':
                    token = Token::period;
                    read();
                    break;
                case '(':
                    read();
                    if (ch_ == '*') {
                        comment();
                        token = nextToken();
                    } else {
                        token = Token::lparen;
                    }
                    break;
                case ')':
                    token = Token::rparen;
                    read();
                    break;
                case '[':
                    token = Token::lbrack;
                    read();
                    break;
                case ']':
                    token = Token::rbrack;
                    read();
                    break;
                case '~':
                    token = Token::op_not;
                    read();
                    break;
                case '"':
                    token = Token::const_string;
                    strValue_ = string();
                    read();
                    break;
                default:
                    token = Token::null;
                    read();
                    break;
            }
        }
    } else {
        token = Token::eof;
    }
    return token;
}

const Token Scanner::peekToken() {
    if (token_ == Token::null) {
        token_ = nextToken();
    }
    return token_;
}

void Scanner::read() {
    if (ch_ == '\n') {
        lineNo_++;
        charNo_ = 0;
    }
    if (file_.get(ch_)) {
        charNo_++;
    } else {
        if (file_.eof()) {
            ch_ = -1;
        } else {
            // TODO I/O Exception
            logError("Error reading file.");
        }
    }
}

void Scanner::logError(const std::string &error) {
    std::cerr << filename_ << ":" << lineNo_ << ":" << charNo_ << ": error: " << error << std::endl;
}

void Scanner::comment() {
    read();
    while (true) {
        while (true) {
            while (ch_ == '(') {
                read();
                if (ch_ == '*') {
                    comment();
                }
            }
            if (ch_ == '*') {
                read();
                break;
            }
            if (ch_ == -1) {
                break;
            }
            read();
        }
        if (ch_ == ')') {
            read();
            break;
        }
        if (ch_ == -1) {
            logError("Comment not terminated.");
            break;
        }
    }
}

const Token Scanner::ident() {
    Token token = Token::const_ident;
    std::stringstream ss;
    do {
        ss << ch_;
        read();
    } while (((ch_ >= '0') && (ch_ <= '9')) ||
             ((ch_ >= 'a') && (ch_ <= 'z')) ||
             ((ch_ >= 'A') && (ch_ <= 'Z')));
    ident_ = ss.str();
    std::unordered_map<std::string, Token>::const_iterator it = keywords_.find(ident_);
    if (it != keywords_.end()) {
        token = it->second;
        ident_ = "";
    }
    return token;
}

const int Scanner::number() {
    bool isHex = false;
    int decValue = 0;
    int hexValue = 0;
    do {
        isHex = isHex | ((ch_ >= 'A') && (ch_ <= 'F'));
        if (decValue <= ((INT_MAX - ch_ + '0') / 10)) {
            if ((ch_ >= '0') && (ch_ <= '9')) {
                decValue = 10 * decValue + (ch_ - '0');
                hexValue = 16 * hexValue + (ch_ - '0');
            } else { // A - F
                hexValue = 16 * hexValue + (ch_ - 'A' + 10);
            }
        } else {
            logError("Number too large.");
            return 0;
        }
        read();
    } while (((ch_ >= '0') && (ch_ <= '9')) ||
             ((ch_ >= 'A') && (ch_ <= 'F')));
    if (ch_ == 'H') {
        // hexadecimal number identified by trailing 'H'
        isHex = true;
        read();
    }
    if (isHex) {
        return hexValue;
    }
    return decValue;
}

const std::string Scanner::string() {
    std::stringstream ss;
    do {
        ss << ch_;
        if (ch_ == '\\') {
            read();
            ss << ch_;
        }
        read();
    } while (ch_ != '"');
    ss << ch_;
    std::string s = ss.str();
    return s;
}