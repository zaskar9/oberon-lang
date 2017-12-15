//
// Created by Michael Grossniklaus on 12/15/17.
//

#include "Scanner.h"
#include <iostream>

Scanner::Scanner(const char *filename)
{
    file.open(filename, std::ios::in);
    if (!file.is_open())
    {
        std::cout << "Cannot open file." << std::endl;
        exit(1);
    }
    char ch;
    file.get(ch);
    std::cout << ch << std::endl;
}
