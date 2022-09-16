/*
 * Logger used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "Logger.h"
#include <iomanip>

void Logger::log(const LogLevel level, const std::string &fileName, int lineNo, int charNo,
                 const std::string &msg) {
    counts_[(unsigned int) level]++;
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
        *out << "[" ;
        switch (level) {
            case LogLevel::DEBUG:   *out << "debug";   break;
            case LogLevel::INFO:    *out << "info";    break;
            case LogLevel::WARNING: *out << "warning"; break;
            case LogLevel::ERROR:   *out << "error";   break;
            default: break; // do nothing
        }
        *out << "] " << msg << std::endl;
    }
}

void Logger::log(const LogLevel level, const std::string &fileName, const std::string &msg) {
    log(level, fileName, -1, -1, msg);
}

void Logger::error(const FilePos &pos, const std::string &msg) {
    log(LogLevel::ERROR, pos.fileName, pos.lineNo, pos.charNo, msg);
}

void Logger::error(const std::string &fileName, const std::string &msg) {
    log(LogLevel::ERROR, fileName, msg);
}

void Logger::warning(const FilePos &pos, const std::string &msg) {
    log(LogLevel::WARNING, pos.fileName, pos.lineNo, pos.charNo, msg);
}

void Logger::warning(const std::string &fileName, const std::string &msg) {
    log(LogLevel::WARNING, fileName, msg);
}

void Logger::info(const std::string &fileName, const std::string &msg) {
    log(LogLevel::INFO, fileName, msg);
}

void Logger::debug(const std::string &fileName, const std::string &msg) {
    log(LogLevel::DEBUG, fileName, msg);
}

int Logger::getDebugCount() const {
    return counts_[(unsigned int) LogLevel::DEBUG];
}

int Logger::getInfoCount() const {
    return counts_[(unsigned int) LogLevel::INFO];
}

int Logger::getWarningCount() const {
    return counts_[(unsigned int) LogLevel::WARNING];
}

int Logger::getErrorCount() const {
    return counts_[(unsigned int) LogLevel::ERROR];
}

void Logger::setLevel(LogLevel level) {
    level_ = level;
}
