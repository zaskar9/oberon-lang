/*
 * Logger used by the Oberon LLVM compiler.
 *
 * Created by Michael Grossniklaus on 2/8/18.
 */

#include "Logger.h"

#include <iostream>
#include <string>
#include "config.h"

using std::ostream;
using std::string;

void Logger::log(LogLevel level, const string &fileName, int lineNo, int charNo, const string &msg) {
    if (werror_ && level == LogLevel::WARNING) {
        level = LogLevel::ERROR;
    }
    counts_[(unsigned int) level]++;
    if (level >= level_) {
        ostream &out = (level == LogLevel::ERROR) ? err_ : out_;
        if (!fileName.empty()) {
            out << fileName;
            if (lineNo >= 0) {
                out << ":" << lineNo;
                if (charNo >= 0) {
                    out << ":" << charNo;
                }
            }
            out << ": ";
        }
        switch (level) {
            case LogLevel::WARNING: out << "\u001b[1m\u001b[95mwarning: \u001b[97m"; break;
            case LogLevel::ERROR:   out << "\u001b[1m\u001b[91merror: \u001b[97m";   break;
            default: break; // do nothing
        }
        out << msg << "\u001b[0m" << std::endl;
    }
}

void Logger::log(const LogLevel level, const string &fileName, const string &msg) {
    log(level, fileName, -1, -1, msg);
}

void Logger::log(const LogLevel level, const string &msg) {
    log(level, {}, msg);
}

void Logger::error(const FilePos &pos, const string &msg) {
    log(LogLevel::ERROR, pos.fileName, pos.lineNo, pos.charNo, msg);
}

void Logger::error(const string &fileName, const string &msg) {
    if (fileName.empty()) {
        log(LogLevel::ERROR, PROJECT_NAME, msg);
    } else {
        log(LogLevel::ERROR, fileName, msg);
    }

}

void Logger::warning(const FilePos &pos, const string &msg) {
    log(LogLevel::WARNING, pos.fileName, pos.lineNo, pos.charNo, msg);
}

void Logger::warning(const string &fileName, const string &msg) {
    if (fileName.empty()) {
        log(LogLevel::WARNING, PROJECT_NAME, msg);
    } else {
        log(LogLevel::WARNING, fileName, msg);
    }
}

void Logger::info(const string &msg) {
    log(LogLevel::INFO, msg);
}

void Logger::debug(const string &msg) {
    log(LogLevel::DEBUG, msg);
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

void Logger::setWarnAsError(bool werror) {
    werror_ = werror;
}
