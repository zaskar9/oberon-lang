//
// Created by Michael Grossniklaus on 3/19/22.
//

#include <iostream>
#include "SymbolFile.h"

void SymbolFile::open(const std::string &path, std::ios::openmode mode) {
    file_.open(path, mode | std::ios::binary);
    path_ = path;
}

std::string SymbolFile::path() const {
    return path_;
}

char SymbolFile::readChar() {
    char val;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << (int) val << "|";
#endif
    return val;
}

void SymbolFile::writeChar(char val) {
#ifdef _DEBUG
    std::cout << (int) val << "|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

int SymbolFile::readInt() {
    int val;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "|";
#endif
    return val;
}

void SymbolFile::writeInt(int val) {
#ifdef _DEBUG
    std::cout << val << "|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

long SymbolFile::readLong() {
    long val;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "|";
#endif
    return val;
}

void SymbolFile::writeLong(long val) {
#ifdef _DEBUG
    std::cout << val << "|";
#endif
    file_.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

std::string SymbolFile::readString() {
    unsigned long len;
    file_.read(reinterpret_cast<char *>(&len), sizeof(len));
#ifdef _DEBUG
    std::cout << len << ":";
#endif
    char *buffer = new char[len];
    file_.read(buffer, (long) len);
    // return {buffer, len};
    auto val = std::string(buffer, len);
#ifdef _DEBUG
    std::cout << val << "|";
#endif
    return val;
}

void SymbolFile::writeString(const std::string &val) {
    unsigned long len = val.length();
#ifdef _DEBUG
    std::cout << len << ":";
#endif
    file_.write(reinterpret_cast<const char*>(&len), sizeof(len));
#ifdef _DEBUG
    std::cout << val << "|";
#endif
    file_.write(val.c_str(), (long) len);
}

bool SymbolFile::eof() {
    return file_.eof();
}

void SymbolFile::flush() {
    file_.flush();
}

void SymbolFile::close() {
    file_.close();
}