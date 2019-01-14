/*
 * Implementation of the logger class used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include <iomanip>
#include "Logger.h"

Logger::Logger() : Logger(LogLevel::ERROR, &std::cout, &std::cerr) {
}

Logger::Logger(LogLevel level, std::ostream *out, std::ostream *err) :
        level_(level), out_(out), err_(err), debugs_(0), infos_(0), warnings_(0), errors_(0) {
}

Logger::~Logger() = default;

void Logger::log(const LogLevel level, const std::string &fileName, int lineNo, int charNo,
                 const std::string &msg) {
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
            case LogLevel::DEBUG:   *out << "DEBUG";   debugs_++;   break;
            case LogLevel::INFO:    *out << "INFO";    infos_++;    break;
            case LogLevel::WARNING: *out << "WARNING"; warnings_++; break;
            case LogLevel::ERROR:   *out << "ERROR";   errors_++;   break;
        }
        *out << "] " << msg << std::endl;
    }
}

void Logger::log(const LogLevel level, const std::string &fileName, const std::string &msg) {
    log(level, fileName, -1, -1, msg);
}

void Logger::error(const FilePos pos, const std::string &msg) {
    log(LogLevel::ERROR, pos.fileName, pos.lineNo, pos.charNo, msg);
}

void Logger::error(const std::string &fileName, const std::string &msg) {
    log(LogLevel::ERROR, fileName, msg);
}

void Logger::warning(FilePos pos, const std::string &msg) {
    log(LogLevel::WARNING, pos.fileName, pos.lineNo, pos.charNo, msg);
}

void Logger::info(const std::string &fileName, const std::string &msg) {
    log(LogLevel::INFO, fileName, msg);
}

void Logger::debug(const std::string &fileName, const std::string &msg) {
    log(LogLevel::DEBUG, fileName, msg);
}

int Logger::getDebugCount() const {
    return debugs_;
}

int Logger::getInfoCount() const {
    return infos_;
}

int Logger::getWarningCount() const {
    return warnings_;
}

int Logger::getErrorCount() const {
    return errors_;
}

void Logger::setLevel(LogLevel level) {
    level_ = level;
}
