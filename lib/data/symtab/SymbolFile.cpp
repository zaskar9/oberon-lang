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

int8_t SymbolFile::readChar() {
    int8_t val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << static_cast<int>(val) << "c|";
#endif
    return val;
}

void SymbolFile::writeChar(const int8_t val) {
#ifdef _DEBUG
    std::cout << static_cast<int>(val) << "c|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

int16_t SymbolFile::readShort() {
    int16_t val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "s|";
#endif
    return val;
}

void SymbolFile::writeShort(const int16_t val) {
#ifdef _DEBUG
    std::cout << val << "s|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

int32_t SymbolFile::readInt() {
    int32_t val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "i|";
#endif
    return val;
}

void SymbolFile::writeInt(const int32_t val) {
#ifdef _DEBUG
    std::cout << val << "i|";
#endif
    file_.write(reinterpret_cast<const char *>(&val), sizeof(val));
}

int64_t SymbolFile::readLong() {
    int64_t val = 0;
    file_.read(reinterpret_cast<char *>(&val), sizeof(val));
#ifdef _DEBUG
    std::cout << val << "l|";
#endif
    return val;
}

void SymbolFile::writeLong(const int64_t val) {
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

void SymbolFile::writeFloat(const float val) {
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

void SymbolFile::writeDouble(const double val) {
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
    const auto buffer = new char[len];
    file_.read(buffer, static_cast<long>(len));
    auto val = std::string(buffer, len);
    delete[] buffer;
#ifdef _DEBUG
    std::cout << (val.empty() ? "0X" : val) << "|";
#endif
    return val;
}

void SymbolFile::writeString(const std::string &val) {
    const auto len = static_cast<unsigned long>(val.length());
#ifdef _DEBUG
    std::cout << len << ":";
#endif
    file_.write(reinterpret_cast<const char*>(&len), sizeof(len));
#ifdef _DEBUG
    std::cout << (val.empty() ? "0X" : val) << "|";
#endif
    file_.write(val.c_str(), static_cast<long>(len));
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