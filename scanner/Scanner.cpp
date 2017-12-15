//
// Created by Michael Grossniklaus on 12/15/17.
//

#include "Scanner.h"
#include <iostream>

Scanner::Scanner(const char *filename)
{
    _file.open(filename, std::ios::in);
    if (!_file.is_open())
    {
        std::cout << "Cannot open file." << std::endl;
        exit(1);
    }
    _lineNo = 1;
    _charNo = 1;
    _ident = "";
    read();
    initTable();
}

Scanner::~Scanner()
{

}

Token Scanner::nextToken()
{
    Token token;
    // Skip whitespace
    while ((_ch != -1) && (_ch <= ' '))
    {
        read();
    }
    if (_ch != -1)
    {
        if (((_ch >= 'A') && (_ch <= 'Z')) || ((_ch >= 'a') && (_ch <= 'z')))
        {
            token = ident();
        }
    }
    return token;
}

int Scanner::getPosition()
{
    return 0;
}

void Scanner::initTable()
{
    _keywords = { { "DIV", Token::op_div }, { "MOD", Token::op_or }, { "OR", Token::op_or },
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

void Scanner::read()
{
    _file.get(_ch);
    _charNo++;
    if (_ch == '\n')
    {
        _lineNo++;
        _charNo = 1;
    }
}

void Scanner::comment()
{
    read();
    while (true)
    {
        while (true)
        {
            while (_ch == '(')
            {
                read();
                if (_ch == '*')
                {
                    comment();
                }
            }
            if (_ch == '*')
            {
                read();
                break;
            }
            if (_ch == -1)
            {
                break;
            }
            read();
        }
        if (_ch == ')')
        {
            read();
            break;
        }
        if (_ch == -1) {
            // TODO exception: comment not terminated
            std::cout << "Comment not terminated." << std::endl;
            break;
        }
    }
}

Token Scanner::ident()
{
    char s[maxIdentifierLen];
    int i = 0;
    Token token = Token::const_ident;
    do
    {
        if (i < maxIdentifierLen)
        {
            s[i++] = _ch;
        }
        read();
    } while (((_ch >= '0') && (_ch <= '9')) ||
             ((_ch >= 'a') && (_ch <= 'z')) ||
             ((_ch >= 'A') && (_ch <= 'Z')));
    _ident = std::string(s);
    if ((token = _keywords[_ident]))
    {
        _ident = "";
    }
    return token;
}