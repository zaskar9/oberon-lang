//
// Created by Michael Grossniklaus on 12/14/17.
//

#include <iostream>
#include "scanner/Scanner.h"

int main(const int argc, const char * argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    std::shared_ptr<Scanner> sc(new Scanner(argv[1]));
    Token token = sc->nextToken();
    while (token != Token::eof)
    {
        token = sc->nextToken();
    }
    return 0;
}