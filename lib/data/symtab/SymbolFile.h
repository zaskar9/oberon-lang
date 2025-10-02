//
// Created by Michael Grossniklaus on 3/19/22.
//

#ifndef OBERON_LANG_SYMBOLFILE_H
#define OBERON_LANG_SYMBOLFILE_H


#include <cstdint>
#include <fstream>
#include <string>

using std::fstream;
using std::string;

class SymbolFile {

public:
    SymbolFile() = default;
    ~SymbolFile() = default;

    void open(const string &path, std::ios::openmode mode);
    [[nodiscard]] string path() const;

    [[nodiscard]] int8_t readChar();
    [[nodiscard]] int16_t readShort();
    [[nodiscard]] int32_t readInt();
    [[nodiscard]] int64_t readLong();
    [[nodiscard]] float readFloat();
    [[nodiscard]] double readDouble();
    [[nodiscard]] std::string readString();

    void writeChar(int8_t val);
    void writeShort(int16_t val);
    void writeInt(int32_t val);
    void writeLong(int64_t val);
    void writeFloat(float val);
    void writeDouble(double val);
    void writeString(const std::string &val);

    [[nodiscard]] bool eof() const;
    void flush();
    void close();

    static constexpr uint32_t VERSION = 5;

private:
    fstream file_;
    string path_;

};


#endif //OBERON_LANG_SYMBOLFILE_H
