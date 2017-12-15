//
// Created by Michael Grossniklaus on 12/14/17.
//

#include <iostream>
#include "scanner/Scanner.h"

int main(int argc, const char * argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    Scanner sc(argv[1]);
    return 0;
}