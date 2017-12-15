//
// Created by Michael Grossniklaus on 12/15/17.
//

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H

#include <fstream>

class Scanner
{

private:
    std::ifstream file;

public:
    explicit Scanner(const char *filename);

};


#endif //OBERON0C_SCANNER_H
