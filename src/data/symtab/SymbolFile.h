//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_SYMBOLFILE_H
#define OBERON_LANG_SYMBOLFILE_H


#include <fstream>

class SymbolFile {

private:
    std::fstream file_;

public:
    explicit SymbolFile() : file_() {};
    ~SymbolFile() = default;

    void open(const std::string &path, std::ios::openmode mode);

    char readChar();
    int readInt();
    long readLong();
    std::string readString();

    void writeChar(char val);
    void writeInt(int val);
    void writeLong(long val);
    void writeString(const std::string &val);

    bool eof();
    void flush();
    void close();

    static const int VERSION = 1;

};


#endif //OBERON_LANG_SYMBOLFILE_H
