/*
 * Implementation of the scanner class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#include "Scanner.h"
#include <iostream>
#include <sstream>

Scanner::Scanner(const std::string &filename, Logger *logger) :
        filename_(filename), logger_(logger), ident_(), numValue_(-1), lineNo_(1), charNo_(0) {
    token_.type = TokenType::null;
    token_.pos = getPosition();
    initTable();
    file_.open(filename_, std::ios::in);
    if (!file_.is_open()) {
        // TODO I/O Exception
        logger_->error(filename_, "Cannot open file.");
        exit(1);
    }
    read();
}

Scanner::~Scanner() {
    if (file_.is_open()) {
        file_.close();
    }
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

void Scanner::initTable() {
    keywords_ = { { "DIV", TokenType::op_div }, { "MOD", TokenType::op_mod }, { "OR", TokenType::op_or },
                  { "MODULE", TokenType::kw_module }, { "PROCEDURE", TokenType::kw_procedure },
                  { "BEGIN", TokenType::kw_begin }, { "END", TokenType::kw_end },
                  { "WHILE", TokenType::kw_while }, { "DO", TokenType::kw_do},
                  { "IF", TokenType::kw_if }, { "THEN", TokenType::kw_then },
                  { "ELSE", TokenType::kw_else }, { "ELSIF", TokenType::kw_elsif },
                  { "VAR", TokenType::kw_var }, { "CONST", TokenType::kw_const },
                  { "TYPE", TokenType::kw_type }, { "ARRAY", TokenType::kw_array },
                  { "RECORD", TokenType::kw_record }, { "OF", TokenType::kw_of },
                  { "TRUE", TokenType::const_true }, { "FALSE", TokenType::const_false } };
}

const Token Scanner::nextToken() {
    Token token;
    if (token_.type != TokenType::null) {
        token = token_;
        token_.type = TokenType::null;
        return token;
    }
    numValue_ = -1;
    ident_ = "";
    // Skip whitespace
    while ((ch_ != -1) && (ch_ <= ' ')) {
        read();
    }
    if (ch_ != -1) {
        token.pos = getPosition();
        if (((ch_ >= 'A') && (ch_ <= 'Z')) || ((ch_ >= 'a') && (ch_ <= 'z'))) {
            // Scan identifier
            token = ident();
        } else if ((ch_ >= '0') && (ch_ <= '9')) {
            // Scan number
            token.type = TokenType::const_number;
            numValue_ = number();
        } else {
            switch (ch_) {
                case '&':
                    token.type = TokenType::op_and;
                    read();
                    break;
                case '*':
                    token.type = TokenType::op_mult;
                    read();
                    break;
                case '+':
                    token.type = TokenType::op_plus;
                    read();
                    break;
                case '-':
                    token.type = TokenType::op_minus;
                    read();
                    break;
                case '=':
                    token.type = TokenType::op_eq;
                    read();
                    break;
                case '#':
                    token.type = TokenType::op_neq;
                    read();
                    break;
                case '<':
                    read();
                    if (ch_ == '=') {
                        token.type = TokenType::op_leq;
                        read();
                    } else {
                        token.type = TokenType::op_lt;
                    }
                    break;
                case '>':
                    read();
                    if (ch_ == '=') {
                        token.type = TokenType::op_geq;
                        read();
                    } else {
                        token.type = TokenType::op_gt;
                    }
                    break;
                case ';':
                    token.type = TokenType::semicolon;
                    read();
                    break;
                case ',':
                    token.type = TokenType::comma;
                    read();
                    break;
                case ':':
                    read();
                    if (ch_ == '=') {
                        token.type = TokenType::op_becomes;
                        read();
                    } else {
                        token.type = TokenType::colon;
                    }
                    break;
                case '.':
                    token.type = TokenType::period;
                    read();
                    break;
                case '(':
                    read();
                    if (ch_ == '*') {
                        comment();
                        token = nextToken();
                    } else {
                        token.type = TokenType::lparen;
                    }
                    break;
                case ')':
                    token.type = TokenType::rparen;
                    read();
                    break;
                case '[':
                    token.type = TokenType::lbrack;
                    read();
                    break;
                case ']':
                    token.type = TokenType::rbrack;
                    read();
                    break;
                case '~':
                    token.type = TokenType::op_not;
                    read();
                    break;
                case '"':
                    token.type = TokenType::const_string;
                    strValue_ = string();
                    read();
                    break;
                default:
                    token.type = TokenType::null;
                    read();
                    break;
            }
        }
    } else {
        token.type = TokenType::eof;
    }
    return token;
}

const Token Scanner::peekToken() {
    if (token_.type == TokenType::null) {
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
            logger_->error(filename_, "Error reading file.");
        }
    }
}

const FilePos Scanner::getPosition() const {
    FilePos pos;
    pos.fileName = filename_;
    pos.lineNo = lineNo_;
    pos.charNo = charNo_;
    return pos;
}

void Scanner::comment() {
    FilePos pos = getPosition();
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
            logger_->error(pos, "Comment not terminated.");
            break;
        }
    }
}

const Token Scanner::ident() {
    Token token;
    token.type = TokenType::const_ident;
    token.pos = getPosition();
    std::stringstream ss;
    do {
        ss << ch_;
        read();
    } while (((ch_ >= '0') && (ch_ <= '9')) ||
             ((ch_ >= 'a') && (ch_ <= 'z')) ||
             ((ch_ >= 'A') && (ch_ <= 'Z')));
    ident_ = ss.str();
    std::unordered_map<std::string, TokenType>::const_iterator it = keywords_.find(ident_);
    if (it != keywords_.end()) {
        token.type = it->second;
        ident_ = "";
    }
    return token;
}

const int Scanner::number() {
    bool isHex = false;
    int decValue = 0;
    int hexValue = 0;
    FilePos pos = getPosition();
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
            logger_->error(pos, "Number too large.");
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
