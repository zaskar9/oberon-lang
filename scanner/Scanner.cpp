//
// Created by Michael Grossniklaus on 12/15/17.
//

#include "Scanner.h"
#include <iostream>

Scanner::Scanner(const std::string& filename) : _ident(), _value(-1), _lineNo(1), _charNo(0), _token(Token::null) {
    _filename = filename;
    _file.open(_filename, std::ios::in);
    if (!_file.is_open()) {
        std::cout << "Cannot open file." << std::endl;
        exit(1);
    }
    read();
    initTable();
}

Scanner::~Scanner() {
    if (_file.is_open()) {
        _file.close();
    }
}

const Token Scanner::nextToken() {
    Token token;
    if (_token != Token::null) {
        token = _token;
        _token = Token::null;
        return token;
    }
    _value = -1;
    _ident = "";
    // Skip whitespace
    while ((_ch != -1) && (_ch <= ' ')) {
        read();
    }
    if (_ch != -1) {
        if (((_ch >= 'A') && (_ch <= 'Z')) || ((_ch >= 'a') && (_ch <= 'z'))) {
            // Scan identifier
            token = ident();
        } else if ((_ch >= '0') && (_ch <= '9')) {
            // Scan number
            token = Token::const_number;
            number();
        } else {
            switch (_ch) {
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
                    if (_ch == '=') {
                        token = Token::op_leq;
                        read();
                    } else {
                        token = Token::op_lt;
                    }
                    break;
                case '>':
                    read();
                    if (_ch == '=') {
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
                    if (_ch == '=') {
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
                    if (_ch == '*') {
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
    if (_token == Token::null) {
        _token = nextToken();
    }
    return _token;
}

const int Scanner::getCharNo() const {
    return _charNo;
}

const int Scanner::getLineNo() const {
    return _lineNo;
}

const int Scanner::getValue() const {
    return _value;
}

const std::string Scanner::getIdent() const {
    return _ident;
}

const std::string Scanner::getFileName() const {
    return _filename;
}

void Scanner::initTable() {
    _keywords = { { "DIV", Token::op_div }, { "MOD", Token::op_mod }, { "OR", Token::op_or },
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

void Scanner::read() {
    if (_ch == '\n') {
        _lineNo++;
        _charNo = 0;
    }
    if (_file.get(_ch)) {
        _charNo++;
    } else {
        if (_file.eof()) {
            _ch = -1;
        } else {
            // TODO I/O Exception
            logError("Error reading file.");
        }
    }
}

void Scanner::logError(const std::string &error) {
    std::cerr << _filename << ":" << _lineNo << ":" << _charNo << ": error:" << error << std::endl;
}

void Scanner::comment() {
    read();
    while (true) {
        while (true) {
            while (_ch == '(') {
                read();
                if (_ch == '*') {
                    comment();
                }
            }
            if (_ch == '*') {
                read();
                break;
            }
            if (_ch == -1) {
                break;
            }
            read();
        }
        if (_ch == ')') {
            read();
            break;
        }
        if (_ch == -1) {
            // TODO exception: comment not terminated
            logError("Comment not terminated.");
            break;
        }
    }
}

const Token Scanner::ident() {
    char s[maxIdentifierLen];
    int i = 0;
    Token token = Token::const_ident;
    do {
        if (i < maxIdentifierLen - 1) {
            s[i++] = _ch;
        }
        read();
    } while (((_ch >= '0') && (_ch <= '9')) ||
             ((_ch >= 'a') && (_ch <= 'z')) ||
             ((_ch >= 'A') && (_ch <= 'Z')));
    s[i] = '\0';
    _ident = std::string(s);
    std::unordered_map<std::string, Token>::const_iterator it = _keywords.find(_ident);
    if (it != _keywords.end()) {
        token = it->second;
        _ident = "";
    }
    return token;
}

void Scanner::number() {
    bool isHex = false;
    int hexValue = 0;
    _value = 0;

    do {
        isHex = isHex | ((_ch >= 'A') && (_ch <= 'F'));
        if (_value <= ((INT_MAX - _ch + '0') / 10)) {
            if ((_ch >= '0') && (_ch <= '9')) {
                _value = 10 * _value + (_ch - '0');
                hexValue = 16 * hexValue + (_ch - '0');
            } else { // A - F
                hexValue = 16 * hexValue + (_ch - 'A' + 10);
            }
        } else {
            // TODO exception: number too large
            logError("Number too large.");
            _value = 0;
            hexValue = 0;
        }
        read();
    } while (((_ch >= '0') && (_ch <= '9')) ||
             ((_ch >= 'A') && (_ch <= 'F')));

    if (_ch == 'H') {
        // hexadecimal number identified by trailing 'H'
        isHex = true;
        read();
    }

    if (isHex) {
        _value = hexValue;
    }
}