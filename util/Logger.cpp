/*
 * Implementation of the logger class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iostream>
#include "Logger.h"

Logger::Logger() = default;

Logger::~Logger() = default;

void Logger::error(const FilePos pos, const std::string &msg) const {
    std::cerr << pos.fileName << ":" << pos.lineNo << ":" << pos.charNo << ": [error]: " << msg << std::endl;
}

void Logger::error(const std::string &fileName, const std::string &msg) const {
    std::cerr << fileName << ": [error]: " << msg << std::endl;
}

void Logger::info(const std::string &fileName, const std::string &msg) const {
    std::cout << fileName << ": [info]: " << msg << std::endl;
}
