//
// Created by Michael Grossniklaus on 3/19/22.
//

#include <iostream>
#include "SymbolFile.h"

void SymbolFile::open(const std::string &path, std::ios::openmode mode) {
    file_.open(path, mode | std::ios::binary);
}

char SymbolFile::readChar() {
    char val;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
    return val;
}

void SymbolFile::writeChar(char val) {
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

int SymbolFile::readInt() {
    int val;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
    return val;
}

void SymbolFile::writeInt(int val) {
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

long SymbolFile::readLong() {
    long val;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
    return val;
}

void SymbolFile::writeLong(long val) {
    file_.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

std::string SymbolFile::readString() {
    unsigned long len;
    file_.read(reinterpret_cast<char *>(&len), sizeof(len));
    char *buffer = new char[len];
    file_.read(buffer, (long) len);
    return {buffer, len};
}

void SymbolFile::writeString(const std::string &val) {
    unsigned long len = val.length();
    file_.write(reinterpret_cast<const char*>(&len), sizeof(len));
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