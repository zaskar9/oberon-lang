/*
 * Scanner used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */


#include "Scanner.h"

#include <cctype>
#include <memory>
#include <sstream>
#include <string>

#include <boost/algorithm/string/replace.hpp>
#include <boost/convert.hpp>
#include <boost/convert/stream.hpp>
#include <boost/lexical_cast.hpp>

#include "IdentToken.h"
#include "LiteralToken.h"
#include "UndefinedToken.h"

using std::make_unique;
using std::string;
using std::stringstream;
using std::unique_ptr;

Scanner::Scanner(Logger &logger, const path &path) : logger_(logger), path_(path),
        tokens_(), lineNo_(1), charNo_(0), ch_{}, eof_(false) {
    init();
    file_.open(path_.string(), std::ifstream::binary);
    if (!file_.is_open()) {
        logger_.error(string(), "cannot open file: " + path_.string() + ".");
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
                  { "OR", TokenType::op_or }, { "IN", TokenType::op_in }, { "IS", TokenType::op_is },
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
                  { "CASE", TokenType::kw_case }, { "WITH", TokenType::kw_with },
                  { "VAR", TokenType::kw_var }, { "CONST", TokenType::kw_const },
                  { "TYPE", TokenType::kw_type }, { "ARRAY", TokenType::kw_array },
                  { "RECORD", TokenType::kw_record }, { "OF", TokenType::kw_of },
                  { "POINTER", TokenType::kw_pointer }, { "NIL", TokenType::kw_nil },
                  { "TRUE", TokenType::boolean_literal}, { "FALSE", TokenType::boolean_literal } };
}

const Token* Scanner::peek(bool advance) {
    if (tokens_.empty()) {
        tokens_.push(scanToken());
        return tokens_.back().get();
    }
    if (advance) {
        auto token = tokens_.back().get();
        tokens_.push(scanToken());
        return token;
    } else {
        return tokens_.front().get();
    }
}

unique_ptr<const Token> Scanner::next() {
    if (tokens_.empty()) {
        return scanToken();
    } else {
        auto token = std::move(tokens_.front());
        tokens_.pop();
        return token;
    }
}

void Scanner::seek(const FilePos &pos) {
    file_.seekg(pos.offset - (streampos)1);
    queue<unique_ptr<const Token>> empty;
    std::swap(tokens_, empty);
    lineNo_ = pos.lineNo;
    charNo_ = pos.charNo - 1;
    ch_ = '\0';
}

unique_ptr<const Token> Scanner::scanToken() {
    // skip whitespace
    while (!eof_ && std::isspace(ch_)) {
        read();
    }
    FilePos pos = current();
    if (!eof_) {
        if (std::isalpha(ch_) || ch_ == '_') {
            return scanIdent();
        } else if (std::isdigit(ch_)) {
            return scanNumber();
        } else if (ch_ == '"') {
            return scanString();
        } else if (ch_ == '\'') {
            return scanCharacter();
        } else {
            switch (ch_) {
                case '&':
                    read();
                    return make_unique<Token>(TokenType::op_and, pos);
                case '*':
                    read();
                    return make_unique<Token>(TokenType::op_times, pos);
                case '/':
                    read();
                    return make_unique<Token>(TokenType::op_divide, pos);
                case '+':
                    read();
                    return make_unique<Token>(TokenType::op_plus, pos);
                case '-':
                    read();
                    return make_unique<Token>(TokenType::op_minus, pos);
                case '=':
                    read();
                    return make_unique<Token>(TokenType::op_eq, pos);
                case '#':
                    read();
                    return make_unique<Token>(TokenType::op_neq, pos);
                case '<':
                    read();
                    if (ch_ == '=') {
                        read();
                        return make_unique<Token>(TokenType::op_leq, pos, current());
                    }
                    return make_unique<Token>(TokenType::op_lt, pos);
                case '>':
                    read();
                    if (ch_ == '=') {
                        read();
                        return make_unique<Token>(TokenType::op_geq, pos, current());
                    }
                    return make_unique<Token>(TokenType::op_gt, pos);
                case ';':
                    read();
                    return make_unique<Token>(TokenType::semicolon, pos);
                case ',':
                    read();
                    return make_unique<Token>(TokenType::comma, pos);
                case ':':
                    read();
                    if (ch_ == '=') {
                        read();
                        return make_unique<Token>(TokenType::op_becomes, pos, current());
                    }
                    return make_unique<Token>(TokenType::colon, pos);
                case '.':
                    read();
                    if (!eof_ && ch_ == '.') {
                        FilePos nextPos = current();
                        read();
                        if (!eof_ && ch_ == '.') {
                            read();
                            return make_unique<Token>(TokenType::varargs, pos, current());
                        }
                        return make_unique<Token>(TokenType::range, pos, current());
                    }
                    return make_unique<Token>(TokenType::period, pos, current());
                case '(':
                    pos = current();
                    read();
                    if (ch_ == '*') {
                        scanComment(pos);
                        return scanToken();
                    }
                    return make_unique<Token>(TokenType::lparen, pos);
                case ')':
                    read();
                    return make_unique<Token>(TokenType::rparen, pos);
                case '[':
                    read();
                    return make_unique<Token>(TokenType::lbrack, pos);
                case ']':
                    read();
                    return make_unique<Token>(TokenType::rbrack, pos);
                case '{':
                    read();
                    return make_unique<Token>(TokenType::lbrace, pos);
                case '}':
                    read();
                    return make_unique<Token>(TokenType::rbrace, pos);
                case '^':
                    read();
                    return make_unique<Token>(TokenType::caret, pos);
                case '|':
                    read();
                    return make_unique<Token>(TokenType::pipe, pos);
                case '~':
                    read();
                    return make_unique<Token>(TokenType::op_not, pos);
                default:
                    read();
                    logger_.error(pos, "bad character.");
                    return scanToken();
            }
        }
    } else {
        return make_unique<Token>(TokenType::eof, current());
    }
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
        eof_ = true;
    } else {
        logger_.error(path_.string(), "error reading file.");
        exit(1);
    }

}

FilePos Scanner::current() {
    FilePos pos;
    pos.fileName = path_.string();
    pos.lineNo = lineNo_;
    pos.charNo = charNo_;
    pos.offset = file_.tellg();
    return pos;
}

void Scanner::scanComment(const FilePos &pos) {
    read();
    while (true) {
        while (true) {
            while (ch_ == '(') {
                auto npos = current();
                read();
                if (ch_ == '*') {
                    scanComment(npos);
                }
            }
            if (ch_ == '*') {
                read();
                break;
            }
            if (eof_) {
                logger_.error(pos, "comment not closed.");
                exit(1);
            }
            read();
        }
        if (ch_ == ')') {
            read();
            break;
        }
        if (eof_) {
            logger_.error(pos, "comment not closed.");
            exit(1);
        }
    }
}

unique_ptr<const Token> Scanner::scanIdent() {
    FilePos pos = current();
    std::stringstream ss;
    do {
        ss << ch_;
        read();
    } while (!eof_ && (std::isalnum(ch_) || ch_ == '_'));
    std::string ident = ss.str();
    auto it = keywords_.find(ident);
    if (it != keywords_.end()) {
        if (it->second == TokenType::boolean_literal) {
            return make_unique<BooleanLiteralToken>(pos, current(), it->first == "TRUE");
        }
        return make_unique<Token>(it->second, pos, current());
    }
    return make_unique<IdentToken>(pos, current(), ident);
}

unique_ptr<const Token> Scanner::scanNumber() {
    bool isHex = false;
    bool isFloat = false;
    bool isChar = false;
    FilePos pos = current();
    std::stringstream ss;
    while (!eof_ && ((ch_ >= '0' && ch_ <= '9') || (toupper(ch_) >= 'A' && toupper(ch_) <= 'F'))) {
        ss << ch_;
        read();
        if (ch_ == '.') {
            auto offset = file_.tellg();
            read();
            if (ch_ == '.') {
                file_.seekg(offset);
                break;
            }
            ss << '.';
            isFloat = true;
        }
        if (isFloat && (ch_ == '+' || ch_ == '-')) {
            ss << ch_;
            read();
        }
    }
    if (ch_ == 'H' || ch_ == 'X') {
        if (isFloat) {
            logger_.error(pos, "undefined number.");
            auto token = make_unique<UndefinedToken>(pos, ch_);
            read();
            return token;
        }
        if (ch_ == 'H') {
            isHex = true;
        } else {
            isChar = true;
        }
        read();
    }
    boost::cnv::cstream ccnv;
    auto num = ss.str();
    if (isFloat) {
        double value;
        try {
            auto result = boost::convert<float>(num, ccnv(std::dec)(std::scientific));
            if (result) {
                if (result.value() == 0) {
                    value = boost::convert<double>(num, ccnv(std::dec)(std::scientific)).value();
                    if (value != 0) {
                        return make_unique<DoubleLiteralToken>(pos, current(), value);
                    }
                }
                return make_unique<FloatLiteralToken>(pos, current(), result.value());
            }
            value = boost::convert<double>(num, ccnv(std::dec)(std::scientific)).value();
        } catch (boost::bad_optional_access const &) {
            logger_.error(pos, "invalid floating-point literal: " + num + ".");
            value = 0;
        }
        return make_unique<DoubleLiteralToken>(pos, current(), value);
    } else if (isChar) {
        uint8_t value;
        auto result = boost::convert<uint32_t>(num, ccnv(std::hex));
        if (result && result.value() < 256) {
            value = static_cast<uint8_t>(result.value());
        } else {
            logger_.error(pos, "invalid character literal: " + num + ".");
            value = 0;
        }
        return make_unique<CharLiteralToken>(pos, current(), value);
    } else {
        bool isLong = true;
        bool isInt = true;
        int64_t value;
        try {
            if (isHex) {
                auto res = boost::convert<uint64_t>(num, ccnv(std::hex)).value();
                // Hexadecimal integers are unsigned
                isLong = res > std::numeric_limits<uint32_t>::max();
                isInt = res > std::numeric_limits<uint16_t>::max();
                value = static_cast<int64_t>(res);
            } else {
                auto res = boost::convert<uint64_t>(num, ccnv(std::dec)).value();
                // Decimal integers are signed
                isLong = res > std::numeric_limits<int32_t>::max();
                isInt = res > std::numeric_limits<int16_t>::max();
                value = static_cast<int64_t>(res);
            }
        } catch (boost::bad_optional_access const &) {
            logger_.error(pos, "invalid integer literal: " + num + ".");
            value = 0;
        }
        if (isLong) {
            return make_unique<LongLiteralToken>(pos, current(), value);
        } else if (isInt) {
            return make_unique<IntLiteralToken>(pos, current(), static_cast<int32_t>(value));
        } else {
            return make_unique<ShortLiteralToken>(pos, current(), static_cast<int16_t>(value));
        }
    }
}

unique_ptr<const Token> Scanner::scanCharacter() {
    char ch = '\0';
    auto pos = current();
    read();
    if (ch_ != '\'') {
        ch = ch_;
        read();
    }
    if (ch_ == '\'') {
        read();
        return make_unique<CharLiteralToken>(pos, current(), static_cast<unsigned char>(ch));
    }
    logger_.error(pos, "unterminated character literal.");
    return make_unique<UndefinedToken>(pos, ch_);
}

unique_ptr<const Token> Scanner::scanString() {
    stringstream ss;
    auto pos = current();
    read();
    while (ch_ != '"') {
        ss << ch_;
        if (ch_ == '\\') {
            read();
            ss << ch_;
        }
        read();
    }
    read();
    string str = unescape(ss.str());
    if (str.length() <= 1) {
        unsigned char value = str.empty() ? '\0' : static_cast<unsigned char>(str[0]);
        return make_unique<CharLiteralToken>(pos, current(), value);
    }
    return make_unique<StringLiteralToken>(pos, current(), str);
}

std::string Scanner::escape(std::string str) {
    boost::replace_all(str, "\0", "\\0");
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
    boost::replace_all(str, "\\0", "\0");
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
