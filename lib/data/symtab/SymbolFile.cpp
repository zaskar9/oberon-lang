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

signed char SymbolFile::readChar() {
    signed char val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << (int) val << "c|";
#endif
    return val;
}

void SymbolFile::writeChar(signed char val) {
#ifdef _DEBUG
    std::cout << (int) val << "c|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

short SymbolFile::readShort() {
    short val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "s|";
#endif
    return val;
}

void SymbolFile::writeShort(short val) {
#ifdef _DEBUG
    std::cout << val << "s|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

int SymbolFile::readInt() {
    int val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "i|";
#endif
    return val;
}

void SymbolFile::writeInt(int val) {
#ifdef _DEBUG
    std::cout << val << "i|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

long SymbolFile::readLong() {
    long val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "l|";
#endif
    return val;
}

void SymbolFile::writeLong(long val) {
#ifdef _DEBUG
    std::cout << val << "l|";
#endif
    file_.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

float SymbolFile::readFloat() {
    float val = 0.0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "f|";
#endif
    return val;
}

void SymbolFile::writeFloat(float val) {
#ifdef _DEBUG
    std::cout << val << "f|";
#endif
    file_.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

double SymbolFile::readDouble() {
    double val = 0.0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "d|";
#endif
    return val;
}

void SymbolFile::writeDouble(double val) {
#ifdef _DEBUG
    std::cout << val << "d|";
#endif
    file_.write(reinterpret_cast<const char*>(&val), sizeof(val));
}

std::string SymbolFile::readString() {
    unsigned long len = 0;
    file_.read(reinterpret_cast<char *>(&len), sizeof(len));
#ifdef _DEBUG
    std::cout << len << ":";
#endif
    char *buffer = new char[len];
    file_.read(buffer, (long) len);
    auto val = std::string(buffer, len);
    delete[] buffer;
#ifdef _DEBUG
    std::cout << (val.empty() ? "0X" : val) << "|";
#endif
    return val;
}

void SymbolFile::writeString(const std::string &val) {
    unsigned long len = static_cast<unsigned long>(val.length());
#ifdef _DEBUG
    std::cout << len << ":";
#endif
    file_.write(reinterpret_cast<const char*>(&len), sizeof(len));
#ifdef _DEBUG
    std::cout << (val.empty() ? "0X" : val) << "|";
#endif
    file_.write(val.c_str(), (long) len);
}

bool SymbolFile::eof() const {
    return file_.eof();
}

void SymbolFile::flush() {
    file_.flush();
}

void SymbolFile::close() {
    file_.close();
}