//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_SYMBOLFILE_H
#define OBERON_LANG_SYMBOLFILE_H


#include <fstream>

class SymbolFile {

private:
    std::fstream file_;
    std::string path_;

public:
    explicit SymbolFile() : file_(), path_() {};
    ~SymbolFile() = default;

    void open(const std::string &path, std::ios::openmode mode);
    [[nodiscard]] std::string path() const;

    [[nodiscard]] signed char readChar();
    [[nodiscard]] int readInt();
    [[nodiscard]] long readLong();
    [[nodiscard]] float readFloat();
    [[nodiscard]] double readDouble();
    [[nodiscard]] std::string readString();

    void writeChar(signed char val);
    void writeInt(int val);
    void writeLong(long val);
    void writeFloat(float val);
    void writeDouble(double val);
    void writeString(const std::string &val);

    [[nodiscard]] bool eof();
    void flush();
    void close();

    static const int VERSION = 1;

};


#endif //OBERON_LANG_SYMBOLFILE_H
