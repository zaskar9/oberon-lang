/*
 * Scanner used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */


#include <boost/algorithm/string/replace.hpp>
#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>
#include <config.h>
#include "Scanner.h"
#include "IdentToken.h"
#include "LiteralToken.h"
#include "UndefinedToken.h"

Scanner::Scanner(const boost::filesystem::path &path, Logger *logger) :
        filename_(path.string()), logger_(logger), tokens_(), lineNo_(1), charNo_(0) {
    init();
    file_.open(filename_, std::ios::in);
    if (!file_.is_open()) {
        logger_->error(PROJECT_NAME, "cannot openNamespace file: " + filename_ + ".");
        exit(1);
    }
    read();
}

Scanner::~Scanner() {
    if (file_.is_open()) {
        file_.close();
    }
}

void Scanner::init() {
    keywords_ = { { "DIV", TokenType::op_div }, { "MOD", TokenType::op_mod },
                  { "OR", TokenType::op_or },
                  { "MODULE", TokenType::kw_module }, { "IMPORT", TokenType::kw_import },
                  { "PROCEDURE", TokenType::kw_procedure }, { "EXTERN", TokenType::kw_extern },
                  { "BEGIN", TokenType::kw_begin }, { "END", TokenType::kw_end },
                  { "RETURN", TokenType::kw_return },
                  { "LOOP", TokenType::kw_loop }, { "EXIT", TokenType::kw_exit },
                  { "WHILE", TokenType::kw_while }, { "DO", TokenType::kw_do },
                  { "REPEAT", TokenType::kw_repeat }, { "UNTIL", TokenType::kw_until },
                  { "FOR", TokenType::kw_for }, { "TO", TokenType::kw_to },
                  { "BY", TokenType::kw_by },
                  { "IF", TokenType::kw_if }, { "THEN", TokenType::kw_then },
                  { "ELSE", TokenType::kw_else }, { "ELSIF", TokenType::kw_elsif },
                  { "VAR", TokenType::kw_var }, { "CONST", TokenType::kw_const },
                  { "TYPE", TokenType::kw_type }, { "ARRAY", TokenType::kw_array },
                  { "RECORD", TokenType::kw_record }, { "OF", TokenType::kw_of },
                  { "POINTER", TokenType::kw_pointer }, { "NIL", TokenType::kw_nil },
                  { "TRUE", TokenType::boolean_literal}, { "FALSE", TokenType::boolean_literal } };
}

const Token* Scanner::peek() {
    if (tokens_.empty()) {
        tokens_.push(scanToken());
    }
    return tokens_.front();
}

std::unique_ptr<const Token> Scanner::next() {
    const Token *token;
    if (tokens_.empty()) {
        token = scanToken();
    } else {
        token = tokens_.front();
        tokens_.pop();
    }
    return std::unique_ptr<const Token>(token);
}

const Token* Scanner::scanToken() {
    const Token *token;
    // Skip whitespace
    while ((ch_ != -1) && (ch_ <= ' ')) {
        read();
    }
    FilePos pos = current();
    if (ch_ != -1) {
        if (((ch_ >= 'A') && (ch_ <= 'Z')) || ((ch_ >= 'a') && (ch_ <= 'z')) || ch_ == '_') {
            // Scan identifier
            token = scanIdent();
        } else if ((ch_ >= '0') && (ch_ <= '9')) {
            // Scan integer
            token = scanNumber();
        } else if (ch_ == '"') {
            token = scanString();
        } else {
            switch (ch_) {
                case '&':
                    read();
                    token = new Token(TokenType::op_and, pos);
                    break;
                case '*':
                    read();
                    token = new Token(TokenType::op_times, pos);
                    break;
                case '+':
                    read();
                    token = new Token(TokenType::op_plus, pos);
                    break;
                case '-':
                    read();
                    token = new Token(TokenType::op_minus, pos);
                    break;
                case '=':
                    read();
                    token = new Token(TokenType::op_eq, pos);
                    break;
                case '#':
                    read();
                    token = new Token(TokenType::op_neq, pos);
                    break;
                case '<':
                    read();
                    if (ch_ == '=') {
                        read();
                        token = new Token(TokenType::op_leq, pos, current());
                    } else {
                        token = new Token(TokenType::op_lt, pos);
                    }
                    break;
                case '>':
                    read();
                    if (ch_ == '=') {
                        read();
                        token = new Token(TokenType::op_geq, pos, current());
                    } else {
                        token = new Token(TokenType::op_gt, pos);
                    }
                    break;
                case ';':
                    read();
                    token = new Token(TokenType::semicolon, pos);
                    break;
                case ',':
                    read();
                    token = new Token(TokenType::comma, pos);
                    break;
                case ':':
                    read();
                    if (ch_ == '=') {
                        read();
                        token = new Token(TokenType::op_becomes, pos, current());
                    } else {
                        token = new Token(TokenType::colon, pos);
                    }
                    break;
                case '.':
                    read();
                    token = new Token(TokenType::period, pos);
                    if (ch_ == '.') {
                        FilePos nextPos = current();
                        read();
                        if (ch_ == '.') {
                            read();
                            token = new Token(TokenType::varargs, pos, current());
                        } else {
                            tokens_.push(new Token(TokenType::period, nextPos));
                        }
                    }
                    break;
                case '(':
                    read();
                    if (ch_ == '*') {
                        scanComment();
                        token = scanToken();
                    } else {
                        token = new Token(TokenType::lparen, pos);
                    }
                    break;
                case ')':
                    read();
                    token = new Token(TokenType::rparen, pos);
                    break;
                case '[':
                    read();
                    token = new Token(TokenType::lbrack, pos);
                    break;
                case ']':
                    read();
                    token = new Token(TokenType::rbrack, pos);
                    break;
                case '{':
                    read();
                    token = new Token(TokenType::lbrace, pos);
                    break;
                case '}':
                    read();
                    token = new Token(TokenType::rbrace, pos);
                    break;
                case '^':
                    read();
                    token = new Token(TokenType::caret, pos);
                    break;
                case '|':
                    read();
                    token = new Token(TokenType::pipe, pos);
                    break;
                case '~':
                    read();
                    token = new Token(TokenType::op_not, pos);
                    break;
                default:
                    read();
                    token = new UndefinedToken(pos, ch_);
                    break;
            }
        }
    } else {
        token = new Token(TokenType::eof, current());
    }
    if (token->type() == TokenType::undef) {
        logger_->error(pos, "bad character.");
        token = scanToken();
    }
    return token;
}

void Scanner::read() {
    if (ch_ == '\n') {
        lineNo_++;
        charNo_ = 0;
    }
    if (file_.get(ch_)) {
        charNo_++;
    } else if (file_.eof()) {
        charNo_++;
        ch_ = -1;
    } else {
        logger_->error(filename_, "error reading file.");
        exit(1);
    }

}

FilePos Scanner::current() const {
    FilePos pos;
    pos.fileName = filename_;
    pos.lineNo = lineNo_;
    pos.charNo = charNo_;
    return pos;
}

void Scanner::scanComment() {
    FilePos pos = current();
    read();
    while (true) {
        while (true) {
            while (ch_ == '(') {
                read();
                if (ch_ == '*') {
                    scanComment();
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
            logger_->error(pos, "comment not terminated.");
            break;
        }
    }
}

const Token* Scanner::scanIdent() {
    FilePos pos = current();
    std::stringstream ss;
    do {
        ss << ch_;
        read();
    } while (((ch_ >= '0') && (ch_ <= '9')) ||
             ((ch_ >= 'a') && (ch_ <= 'z')) ||
             ((ch_ >= 'A') && (ch_ <= 'Z')) || ch_ == '_');
    std::string ident = ss.str();
    auto it = keywords_.find(ident);
    if (it != keywords_.end()) {
        if (it->second == TokenType::boolean_literal) {
            return new BooleanLiteralToken(pos, current(), it->first == "TRUE");
        }
        return new Token(it->second, pos, current());
    }
    return new IdentToken(pos, current(), ident);
}

const Token* Scanner::scanNumber() {
    bool isHex = false;
    bool isFloat = false;
    FilePos pos = current();
    std::stringstream ss;
    while (((ch_ >= '0') && (ch_ <= '9')) ||
           ((toupper(ch_) >= 'A') && (toupper(ch_) <= 'F'))) {
        ss << ch_;
        read();
        if (ch_ == '.') {
            ss << ch_;
            isFloat = true;
            read();
        }
        if (isFloat && (ch_ == '+' || ch_ == '-')) {
            ss << ch_;
            read();
        }
    }
    if (ch_ == 'H') {
        if (isFloat) {
            logger_->error(pos, "undefined number.");
            auto token = new UndefinedToken(pos, ch_);
            read();
            return token;
        }
        isHex = true;
        read();
    }
    boost::cnv::cstream ccnv;
    auto num = ss.str();
    if (isFloat) {
        double value = 0;
        try {
             value = boost::convert<double>(num, ccnv(std::dec)(std::scientific)).value();
        } catch (boost::bad_optional_access const&) {
            logger_->error(pos, "invalid real literal: " + num + ".");
        }
        if (value >= std::numeric_limits<float>::lowest() && value <= std::numeric_limits<float>::max()) {
            return new RealLiteralToken(pos, current(), static_cast<float>(value));
        }
        return new LongrealLiteralToken(pos, current(), value);
    } else {
        long value;
        try {
            if (isHex) {
                value = boost::convert<long>(num, ccnv(std::hex)).value();
            } else {
                value = boost::convert<long>(num, ccnv(std::dec)).value();
            }
        } catch (boost::bad_optional_access const&) {
            logger_->error(pos, "invalid integer literal: " + num + ".");
            value = 0;
        }
        if (value >= std::numeric_limits<int>::lowest() && value <= std::numeric_limits<int>::max()) {
            return new IntegerLiteralToken(pos, current(), static_cast<int>(value));
        }
        return new LongintLiteralToken(pos, current(), value);
    }
}

const Token* Scanner::scanString() {
    std::stringstream ss;
    auto p = current();
    read();
    do {
        ss << ch_;
        if (ch_ == '\\') {
            read();
            ss << ch_;
        }
        read();
    } while (ch_ != '"');
    read();
    std::string str = ss.str();
    return new StringLiteralToken(p, current(), unescape(str));
}

std::string Scanner::escape(std::string str) {
    boost::replace_all(str, "\'", "\\'");
    boost::replace_all(str, "\"", "\\\"");
    boost::replace_all(str, "\?", "\\?");
    boost::replace_all(str, "\\", "\\\\");
    boost::replace_all(str, "\a", "\\a");
    boost::replace_all(str, "\b", "\\b");
    boost::replace_all(str, "\f", "\\f");
    boost::replace_all(str, "\n",  "\\n");
    boost::replace_all(str, "\r", "\\r");
    boost::replace_all(str, "\t", "\\t");
    boost::replace_all(str, "\v", "\\v");
    return str;

}

std::string Scanner::unescape(std::string str) {
    boost::replace_all(str, "\\'", "\'");
    boost::replace_all(str, "\\\"", "\"");
    boost::replace_all(str, "\\?", "\?");
    boost::replace_all(str, "\\\\", "\\");
    boost::replace_all(str, "\\a", "\a");
    boost::replace_all(str, "\\b", "\b");
    boost::replace_all(str, "\\f", "\f");
    boost::replace_all(str, "\\n",  "\n");
    boost::replace_all(str, "\\r", "\r");
    boost::replace_all(str, "\\t", "\t");
    boost::replace_all(str, "\\v", "\v");
    return str;
}
