/*
 * Implementation of the logger class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iomanip>
#include "Logger.h"

Logger::Logger() : Logger(LogLevel::ERROR, &std::cout, &std::cerr) {
}

Logger::Logger(LogLevel level, std::ostream *out, std::ostream *err) : level_(level), out_(out), err_(err) {
}

Logger::~Logger() = default;

void Logger::log(const LogLevel level, const std::string &fileName, int lineNo, int charNo,
                 const std::string &msg) const {
    if (level >= level_) {
        std::ostream *out = (level == LogLevel::ERROR) ? err_ : out_;
        if (!fileName.empty()) {
            *out << fileName;
            if (lineNo >= 0) {
                *out << ":" << lineNo;
                if (charNo >= 0) {
                    *out << ":" << charNo;
                }
            }
            *out << ": ";
        }
        *out << "[" << std::setw(5);
        switch (level) {
            case LogLevel::DEBUG: *out << "DEBUG"; break;
            case LogLevel::INFO:  *out << "INFO";  break;
            case LogLevel::ERROR: *out << "ERROR"; break;
        }
        *out << "] " << msg << std::endl;
    }
}

void Logger::log(const LogLevel level, const std::string &fileName, const std::string &msg) const {
    log(level, fileName, -1, -1, msg);
}

void Logger::error(const FilePos pos, const std::string &msg) const {
    log(LogLevel::ERROR, pos.fileName, pos.lineNo, pos.charNo, msg);
}

void Logger::error(const std::string &fileName, const std::string &msg) const {
    log(LogLevel::ERROR, fileName, msg);
}

void Logger::info(const std::string &fileName, const std::string &msg) const {
    log(LogLevel::INFO, fileName, msg);
}

void Logger::debug(const std::string &fileName, const std::string &msg) const {
    log(LogLevel::DEBUG, fileName, msg);
}

void Logger::setLevel(LogLevel level) {
    level_ = level;
}
